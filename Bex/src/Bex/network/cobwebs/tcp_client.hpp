#ifndef __BEX_NETWORK_COBWEBS_TCP_CLIENT_H__
#define __BEX_NETWORK_COBWEBS_TCP_CLIENT_H__

//////////////////////////////////////////////////////////////////////////
/// Tcp客户端

/*
* tcp_client是二次封装产物, 用户可以直接使用tcp_connector.
*/

#include "connector/tcp_connector.h"
#include <boost/timer.hpp>
#include <boost/noncopyable.hpp>

namespace Bex { namespace cobwebs
{
    template <class Session>
    class tcp_client
        : public boost::noncopyable
    {
        BOOST_STATIC_ASSERT((boost::is_base_of<tcp_session, Session>::value));

    public:
        typedef Session session_type;
        typedef boost::shared_ptr<Session> session_ptr;
        enum {default_auto_connect_interval = 2 * CLOCKS_PER_SEC, };

    private:
        session_ptr m_session;
        boost::system::error_code m_error;
        CRegisterCore m_register_core;
        options_ptr m_opts;
        group_id m_gid;
        inter_lock m_connect_lock;

        bool m_auto;
        int m_port;
        std::string m_hostname;
        int m_interval;
        boost::timer m_bt;

    protected:
        static const boost::system::error_code s_repeat;

    public:
        explicit tcp_client(options const& opts = options());
        virtual ~tcp_client();

        /// 设置配置选项
        void set_option(options const& opts);

        /// 请求非阻塞连接
        void async_connect(int port, std::string const& hostname);

        /// 请求阻塞式连接
        bool connect(int port, std::string const& hostname);

        /// 获取错误信息
        boost::system::error_code const& get_error_code() const;

        /// 获取session
        session_ptr get_session();

        /// 是否正常连接状态
        bool is_ok();

        /// 设置为自动连接并且断线重连.
        void start_auto_connect(int port, std::string const& hostname, int interval = default_auto_connect_interval);

        /// 停止断线重连
        void stop_auto_connect();

    protected:
        /// 异步连接结果回调
        virtual void on_connect(bool is_success);

        /// 断开连接回调
        virtual void on_disconnect() {}

    private:
        /// 异步连接回调
        void on_connect(tcp_session_ptr ptr, boost::system::error_code const& ec);

        /// 自动连接
        void try_auto_connect();

        /// 清理失效连接
        void run();
    };

    template <class Session>
    const boost::system::error_code tcp_client<Session>::s_repeat = error::already_connected;

    //////////////////////////////////////////////////////////////////////////
    // detail

    template <class Session>
    tcp_client<Session>::tcp_client( options const& opts /*= options()*/ )
    {
        m_auto = false;
        m_opts.reset(new options(opts));
        m_register_core.set(boost::bind(&tcp_client::run, this), m_gid.value());
    }

    template <class Session>
    tcp_client<Session>::~tcp_client()
    {
        // todo something.
        m_register_core.cancel();
    }

    /// 设置配置选项
    template <class Session>
    void tcp_client<Session>::set_option( options const& opts )
    {
        m_opts.reset(new options(opts));
    }

    /// 请求非阻塞连接
    template <class Session>
    void tcp_client<Session>::async_connect( int port, std::string const& hostname )
    {
        if (is_ok() || !m_connect_lock.try_lock())
        {
            m_error = s_repeat;
            return ;
        }

        tcp_connector::getInstance().async_connect<Session>(port, hostname,
            boost::bind(&tcp_client::on_connect, this, _1, _2), m_opts);
    }

    /// 请求阻塞式连接
    template <class Session>
    bool tcp_client<Session>::connect( int port, std::string const& hostname )
    {
        if (is_ok() || !m_connect_lock.try_lock())
        {
            m_error = s_repeat;
            return false;
        }

        tcp_session_ptr ptr = tcp_connector::getInstance().connect<Session>(port,
            hostname, m_error, m_opts);
        on_connect(ptr, boost::system::error_code());
        return is_ok();
    }

    template <class Session> inline 
    boost::system::error_code const& tcp_client<Session>::get_error_code() const
    {
        return m_error;
    }

    template <class Session> inline 
    typename tcp_client<Session>::session_ptr tcp_client<Session>::get_session()
    {
        return m_session;
    }

    /// 是否正常连接状态
    template <class Session>
    bool tcp_client<Session>::is_ok()
    {
        return (m_session && m_session->is_valid());
    }

    /// 设置为自动连接并且断线重连.
    template <class Session>
    void tcp_client<Session>::start_auto_connect( int port, std::string const& hostname, int interval /*= default_auto_connect_interval*/ )
    {
        m_auto = true;
        m_port = port;
        m_hostname = hostname;
        m_interval = interval;
        try_auto_connect();
    }

    /// 停止断线重连
    template <class Session>
    void tcp_client<Session>::stop_auto_connect()
    {
        m_auto = false;
    }

    template <class Session>
    void tcp_client<Session>::on_connect( bool is_success )
    {
        // nothing to do.
    }

    template <class Session>
    void tcp_client<Session>::on_connect( tcp_session_ptr ptr, boost::system::error_code const& ec )
    {
        m_error = ec;
        m_session = boost::static_pointer_cast<session_type>(ptr);
        m_connect_lock.unlock();
        on_connect( is_ok() );
    }

    /// 自动连接
    template <class Session>
    void tcp_client<Session>::try_auto_connect()
    {
        m_bt.restart();
        async_connect(m_port, m_hostname);
    }

    template <class Session>
    void tcp_client<Session>::run()
    {
        if (m_session && !m_session->is_valid())
        {
            on_disconnect();
            m_session.reset();
        }

        if (m_auto && !m_session && m_bt.elapsed() * CLOCKS_PER_SEC > m_interval)
        {
            try_auto_connect();
        }
    }

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_TCP_CLIENT_H__