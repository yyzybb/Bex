#ifndef __BEX_NETWORK_COBWEBS_TCP_ACCEPTOR_H__
#define __BEX_NETWORK_COBWEBS_TCP_ACCEPTOR_H__

//////////////////////////////////////////////////////////////////////////
/// tcp连接接收器

#include <Bex/network/cobwebs/core/core.h>
#include <Bex/network/cobwebs/session/tcp_session.h>
#include <Bex/network/cobwebs/core/options.h>
#include <Bex/network/cobwebs/core/register.hpp>
#include <Bex/utility/lock_ptr.hpp>
#include "acceptor_base.h"
#include <map>

namespace Bex { namespace cobwebs
{
    class tcp_acceptor
        : public acceptor_base
    {
        typedef boost::shared_ptr<tcp::socket> socket_ptr;
        typedef boost::function<tcp_session_ptr(socket_ptr, session_initialized)> FactoryFunc;
        typedef std::map<session_id, tcp_session_ptr> Sessions;

        group_id                m_gid;
        options_ptr             m_opts;           ///< 配置选项
        io_service &            m_ios;
        tcp::acceptor           m_acceptor;       ///< 
        tcp::endpoint           m_endpoint;       ///< 
        FactoryFunc             m_factory;        ///< session工厂函数
        inter_lock              m_lock;           ///< 防止多次启动的锁
        Sessions                m_sessions;       ///< session管理
        boost::recursive_mutex  m_session_mutex;  ///< session管理线程锁
        CRegisterCore           m_register_core;  // -- 构造时要最后一个初始化, 析构时第一个清理.

    public:
        explicit tcp_acceptor(options const& opts = options());
        virtual ~tcp_acceptor();

        /// 设置配置选项
        void set_option(options const& opts);

        /// 启动
        template <class Session>
        bool startup(int port, std::string const& hostname = "0.0.0.0", bool reuse = false)
        {
            return startup(port, session_factory<Session>(), hostname, reuse);
        }

        /// 启动
        template <class Factory>
        bool startup(int port, Factory fact, std::string const& hostname = "0.0.0.0", bool reuse = false)
        {
            m_factory = boost::bind(&invoke_factory<tcp_session, Factory>, fact, _1, _2);
            return startup(port, hostname, reuse);
        }

        /// 优雅地关闭acceptor, 同时关闭所有通过此acceptor建立的session.
        void shutdown();

        /// 强制关闭acceptor
        void close();

        /// 获取sessions
        lock_ptr<const Sessions> get_sessions();

    private:
        /// 启动
        bool startup(int port, std::string const& hostname, bool reuse);

        /// 开始
        void accept();

        /// 连接成功回调
        void on_accept(boost::system::error_code const& ec, socket_ptr pSock);

    private:
        /// 添加新session
        void insert_session(socket_ptr pSock);

        /// 删除session
        void erase_session(session_id sid);

        /// 清理失效连接
        void run();
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_TCP_ACCEPTOR_H__