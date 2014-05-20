#include "tcp_acceptor.h"

namespace Bex { namespace cobwebs
{
    tcp_acceptor::tcp_acceptor(options const& opts /*= options()*/)
        : m_ios(core::getInstance().get_io_service())
        , m_acceptor(m_ios), m_opts(new options(opts))
    {
        m_register_core.set(boost::bind(&tcp_acceptor::run, this), m_gid.value());
    }

    tcp_acceptor::~tcp_acceptor()
    {
        close();
        m_register_core.cancel();
    }

    /// 设置配置选项
    void tcp_acceptor::set_option( options const& opts )
    {
        *m_opts = opts;
    }

    /// 优雅地关闭acceptor, 同时关闭所有通过此acceptor建立的session.
    void tcp_acceptor::shutdown()
    {
        if (!m_lock.is_locked()) return ;

        boost::recursive_mutex::scoped_lock lock(m_session_mutex);
        m_acceptor.close();
        for (Sessions::iterator it = m_sessions.begin();
                it != m_sessions.end(); ++it)
        {
            tcp_session_ptr pSession = (it->second);
            if (pSession)
                pSession->shutdown();
        }
        m_sessions.clear();
        m_lock.unlock();
    }

    /// 强制关闭acceptor
    void tcp_acceptor::close()
    {
        if (!m_lock.is_locked()) return ;

        boost::recursive_mutex::scoped_lock lock(m_session_mutex);
        m_acceptor.close();
        for (Sessions::iterator it = m_sessions.begin();
            it != m_sessions.end(); ++it)
        {
            tcp_session_ptr & pSession = (it->second);
            if (pSession)
                pSession->close();
        }
        m_sessions.clear();
        m_lock.unlock();
    }

    /// 获取sessions
    lock_ptr<const tcp_acceptor::Sessions> tcp_acceptor::get_sessions()
    {
        return lock_ptr<const Sessions>(&m_sessions, m_session_mutex);
    }

    /// 启动
    bool tcp_acceptor::startup( int port, std::string const& hostname, bool reuse )
    {
        //tcp::resolver resolver(m_ios);
        inter_lock::try_scoped lock(m_lock);
        if (!lock.is_locked()) return false;

        try
        {
            m_endpoint = tcp::endpoint(ip::address::from_string(hostname), port);
            m_acceptor.open(m_endpoint.protocol());
            m_acceptor.set_option(socket_base::reuse_address(reuse));
            m_acceptor.bind(m_endpoint);            
            m_acceptor.listen();
            for (int i = 0; i < m_opts->listen_count; ++i)
                accept();
        }
        catch(boost::system::system_error& ec)
        {
            ec;
            return false;
        }

        lock.detach();
        return true;
    }

    /// 开始
    void tcp_acceptor::accept()
    {
        socket_ptr sock(new tcp::socket(m_ios));
        m_acceptor.async_accept( *sock,
            boost::bind(&tcp_acceptor::on_accept, this, placeholders::error, sock) );
    }

    /// 连接成功回调
    void tcp_acceptor::on_accept( boost::system::error_code const& ec, socket_ptr pSock )
    {
        accept();

        if (!ec)
            insert_session(pSock);
    }

    /// 添加新session
    void tcp_acceptor::insert_session( socket_ptr pSock )
    {
        boost::recursive_mutex::scoped_lock lock(m_session_mutex);
        session_id sid;
        tcp_session_ptr ptr = m_factory(pSock, session_initialized(sid, m_gid, m_opts));
        m_sessions[sid] = ptr;
    }

    /// 删除session
    void tcp_acceptor::erase_session( session_id sid )
    {
        boost::recursive_mutex::scoped_lock lock(m_session_mutex);
        m_sessions.erase(sid);
    }

    /// 清理失效连接
    void tcp_acceptor::run()
    {
        boost::recursive_mutex::scoped_lock lock(m_session_mutex);
        Sessions::iterator it = m_sessions.begin();
        while ( it != m_sessions.end() )
        {
            tcp_session_ptr & ptr = it->second;
            if (!ptr->is_valid())
                it = m_sessions.erase(it);
            else
                ++ it;
        }
    }    

} //namespace cobwebs
} //namespace Bex