#ifndef __BEX_NETWORK_COBWEBS_TCP_CONNECTOR_H__
#define __BEX_NETWORK_COBWEBS_TCP_CONNECTOR_H__

//////////////////////////////////////////////////////////////////////////
/// 连接器

#include <Bex/network/cobwebs/core/core.h>
#include <Bex/network/cobwebs/core/options.h>
#include <Bex/network/cobwebs/connector/connector_base.h>
#include <Bex/network/cobwebs/session/tcp_session.h>

namespace Bex { namespace cobwebs
{
    class tcp_connector
        : public connector_base
        , public singleton<tcp_connector>
    {
    public:
        typedef tcp::socket socket;
        typedef boost::shared_ptr<socket> socket_ptr;
        typedef boost::function<tcp_session_ptr(socket_ptr, session_initialized)> FactoryFunc;
        typedef boost::function<void(tcp_session_ptr, boost::system::error_code const&)> ConnectHandler;

    public:
        //////////////////////////////////////////////////////////////////////////
        /// 异步连接
        // @param port 端口号
        // @param hostname 远端地址
        template <class Session>
        void async_connect(int port, std::string const& hostname
            , ConnectHandler const& handler, options const& opts = options())
        {
            async_connect<Session>(port, hostname, handler, options_ptr(new options(opts)));
        }
        template <class Session>
        void async_connect(int port, std::string const& hostname
            , ConnectHandler const& handler, options_ptr opts)
        {
            async_connect(port, hostname, session_factory<Session>(), handler, opts);
        }

        template <class Factory>
        void async_connect(int port, std::string const& hostname
            , Factory fact, ConnectHandler const& handler, options const& opts = options())
        {
            BOOST_STATIC_ASSERT((boost::is_same<typename Factory::socket_ptr, socket_ptr>::value));
            async_connect<Factory>(port, hostname, fact, handler, options_ptr(new options(opts)));
        }
        template <class Factory>
        void async_connect(int port, std::string const& hostname
            , Factory fact, ConnectHandler const& handler, options_ptr opts)
        {
            BOOST_STATIC_ASSERT((boost::is_same<typename Factory::socket_ptr, socket_ptr>::value));
            async_connect(port, hostname, fact_to_function(fact), handler, opts);
        }

        void async_connect(int port, std::string const& hostname
            , FactoryFunc fact_func, ConnectHandler const& handler, options const& opts = options());
        void async_connect(int port, std::string const& hostname
            , FactoryFunc fact_func, ConnectHandler const& handler, options_ptr opts);

        //////////////////////////////////////////////////////////////////////////
        /// 同步连接
        template <class Session>
        tcp_session_ptr connect(int port, std::string const& hostname, boost::system::error_code & ec
            , options const& opts = options())
        {
            return connect<Session>(port, hostname, ec, options_ptr(new options(opts)));
        }
        template <class Session>
        tcp_session_ptr connect(int port, std::string const& hostname, boost::system::error_code & ec
            , options_ptr opts)
        {
            return connect(port, hostname, ec, session_factory<Session>(), opts);
        }

        template <class Factory>
        tcp_session_ptr connect(int port, std::string const& hostname, boost::system::error_code & ec
            , Factory fact, options const& opts = options())
        {
            return connect<Factory>(port, hostname, ec, fact, options_ptr(new options(opts)));
        }
        template <class Factory>
        tcp_session_ptr connect(int port, std::string const& hostname, boost::system::error_code & ec
            , Factory fact, options_ptr opts)
        {
            return connect(fact, port, hostname, ec, opts);
        }

        /// 同步连接
        tcp_session_ptr connect(FactoryFunc fact_func, int port, std::string const& hostname, boost::system::error_code & ec
            , options const& opts = options());
        tcp_session_ptr connect(FactoryFunc fact_func, int port, std::string const& hostname, boost::system::error_code & ec
            , options_ptr opts);

    private:
        /// 连接回调
        void async_connect_handler(boost::system::error_code const& ec
            , socket_ptr pSock, FactoryFunc fact_func, ConnectHandler const& handler
            , options_ptr opts, tcp::resolver::iterator resolver_it);

    public:
        template <class Factory>
        static FactoryFunc fact_to_function(Factory fact)
        {
            return FactoryFunc(boost::bind(&invoke_factory<tcp_session, Factory>, fact, _1, _2));
        }
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_TCP_CONNECTOR_H__