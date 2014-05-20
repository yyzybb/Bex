#include "tcp_session.h"

namespace Bex { namespace cobwebs
{
    tcp_session::tcp_session( socket_ptr pSock, session_initialized const& si )
        : session_base(si), m_socket(pSock), m_notify_connected(true)
        , m_recvbuf(boost::bind(&Pool::alloc_ringbuf, &core::getInstance().get_pool(), _1), boost::bind(&Pool::free_ringbuf, &core::getInstance().get_pool(), _1), m_opts->sendbufbytes, 1, m_opts->sendbufcount)
        , m_sendbuf(boost::bind(&Pool::alloc_ringbuf, &core::getInstance().get_pool(), _1), boost::bind(&Pool::free_ringbuf, &core::getInstance().get_pool(), _1), m_opts->sendbufbytes, 1, m_opts->recvbufcount)
    {
    }

    tcp_session::~tcp_session()
    {
        m_register_core.cancel();
    }

    /// 发送数据
    bool tcp_session::send( char const* buf, std::size_t bytes )
    {
        boost::mutex::scoped_lock lock(m_send_mutex);

        if (m_shutdown_lock.is_locked())
            return false;

        if (m_sendbuf.spare() < bytes)
        {
            if (m_opts->sendbufoverflow_disconnect)
            {
                set_disconnect_error(boost::system::error_code(cec_sendbuf_overflow
                    , cobwebs_error_category::getInstance()));
                shutdown();
            }
            return false;
        }
        
        m_sendbuf.sputn(buf, bytes);
        post_send(false);       ///< 立即推送一次, 以提供发送速度和响应速度.
        return true;
    }

    /// 接收线程推进
    void tcp_session::run()
    {
        if (m_notify_connected)
        {
            connect_invoke();
            m_notify_connected = false;
        }

        post_recv(false);
        post_send(false);

        std::size_t bytes;
        while ((bytes = m_recvbuf.gcount()) > 0)
        {
            on_recv(m_recvbuf.gptr(), bytes);
            m_recvbuf.gbump(bytes);
        }

        try_shutdown();

        if (m_both_closed.is_locked() && m_error)
        {
            on_disconnect(m_error);
            m_error.clear();
            m_valid = false;
        }
    }

    /// 优雅地关闭连接
    void tcp_session::shutdown()
    {
        if (!m_shutdown_lock.try_lock())
            return ;

        set_disconnect_error(boost::system::error_code(cec_driving_shutdown
            , cobwebs_error_category::getInstance()));
    }

    /// 强制关闭
    void tcp_session::close()
    {
        if (!m_both_closed.try_lock())
            return ;

        set_disconnect_error(boost::system::error_code(cec_driving_close
            , cobwebs_error_category::getInstance()));

        boost::system::error_code ec;
        m_socket->cancel(ec);
        m_socket->shutdown(socket_base::shutdown_both, ec);
        //m_socket->close(ec);
    }

    /// 初始化
    void tcp_session::initialize()
    {
        m_register_core.set(boost::bind(&tcp_session::run, this), m_gid.value());
    }

    /// 发起接收请求
    void tcp_session::post_recv(bool relay)
    {
        if (m_receive_closed.is_locked())   /// receive通道已关闭.
            return ;

        if (!relay && !m_receive_relay.try_lock())
            return ;

        std::size_t bytes = m_recvbuf.pcount();
        if (0 == bytes)
        {
            if (m_opts->recvbufoverflow_disconnect)
            {
                set_disconnect_error(boost::system::error_code(cec_recvbuf_overflow
                    , cobwebs_error_category::getInstance()));
                shutdown();
            }

            m_receive_relay.unlock();
            return ;
        }

        bytes = (std::min<std::size_t>)(bytes, 4096);
        m_socket->async_read_some(buffer(m_recvbuf.pptr(), bytes)
            , boost::bind(&tcp_session::recv_handler, shared_from_this()
                , placeholders::error, placeholders::bytes_transferred));
    }

    /// 发起发送请求
    void tcp_session::post_send(bool relay)
    {
        if (m_send_closed.is_locked())  /// send通道已关闭.
            return ;

        if (!relay && !m_send_relay.try_lock())
            return ;

        std::size_t bytes = m_sendbuf.gcount();        
        if (bytes == 0)
        {
            m_send_relay.unlock();
            return ;
        }

        bytes = (std::min<std::size_t>)(bytes, 4096);
        m_socket->async_write_some(buffer(m_sendbuf.gptr(), bytes)
            , boost::bind(&tcp_session::send_handler, shared_from_this()
                , placeholders::error, placeholders::bytes_transferred));
    }

    /// 接收回调
    void tcp_session::recv_handler( boost::system::error_code const& ec, std::size_t bytes )
    {
        if (ec)
        {
            set_disconnect_error(ec);
            passive_shutdown(socket_base::shutdown_receive);
            return ;
        }

        m_recvbuf.pbump(bytes);
        post_recv(true);
    }

    /// 发送回调
    void tcp_session::send_handler( boost::system::error_code const& ec, std::size_t bytes )
    {
        if (ec)
        {
            set_disconnect_error(ec);
            passive_shutdown(socket_base::shutdown_send);
            return ;
        }

        m_sendbuf.gbump(bytes);
        post_send(true);
    }

    /// 连接成功
    void tcp_session::connect_invoke()
    {
        on_connected();
    }

    /// 断开连接
    void tcp_session::set_disconnect_error( boost::system::error_code const& ec )
    {
        if (!m_set_error_lock.try_lock())
            return ;

        m_error = ec;
        BOOST_ASSERT(ec);
    }

    /// 优雅地关闭连接(被动)
    void tcp_session::passive_shutdown(socket_base::shutdown_type what)
    {
        m_shutdown_lock.try_lock();        
        m_socket->shutdown(what);

        switch (what)
        {
        case socket_base::shutdown_receive:
            m_receive_closed.try_lock();
            break;

        case socket_base::shutdown_send:
            m_send_closed.try_lock();
            break;

        case socket_base::shutdown_both:
            m_receive_closed.try_lock();
            m_send_closed.try_lock();
            break;
        }
    }

    /// 尝试关闭连接
    void tcp_session::try_shutdown()
    {
        if (m_shutdown_lock.is_locked())
        {
            if (!m_send_closed.is_locked() && !m_send_relay.is_locked())
                passive_shutdown(socket_base::shutdown_send);
            
            //if (!m_receive_closed.is_locked() && 0 == m_recvbuf.gcount())
            //    passive_shutdown(socket_base::shutdown_receive);

            if (m_send_closed.is_locked() && m_receive_closed.is_locked()
                && 0 == m_recvbuf.gcount())
                close();
        }
    }

} //namespace cobwebs
} //namespace Bex