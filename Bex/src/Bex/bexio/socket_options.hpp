#ifndef __BEX_IO_BEXIO_SOCKET_OPTIONS_HPP__
#define __BEX_IO_BEXIO_SOCKET_OPTIONS_HPP__

//////////////////////////////////////////////////////////////////////////
/// socket选项扩展
#include "bexio_fwd.hpp"

#if defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
#include <MSTCPiP.h>
#endif

namespace Bex { namespace bexio { namespace pltf
{
#if defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
    // windows platform
    inline void set_keepalive_params(boost::asio::detail::socket_type s
        , ULONG idle, ULONG interval, boost::system::error_code & ec)
    {
        DWORD ret_bytes;
        ::tcp_keepalive out = {};
        ::tcp_keepalive in = { TRUE, idle * 1000, interval * 1000 };
        int ret = ::WSAIoctl(s, SIO_KEEPALIVE_VALS
            , &in, sizeof(in), &out, sizeof(out), &ret_bytes, 0, 0);
        if (ret == 0)
            ec.clear();
        else
            ec = boost::system::error_code(::WSAGetLastError()
                , boost::asio::error::get_system_category());
    }
#else 
    // unix platform
    inline void set_keepalive_params(boost::asio::detail::socket_type s
        , int idle, int interval, boost::system::error_code & ec)
    {
        clear_last_error();
        if(::setsockopt(s, SOL_TCP, TCP_KEEPIDLE
            , (void*)&idle, sizeof(idle)) == SOCKET_ERROR)
        {
            ec = boost::system::error_code(errno
                , boost::asio::error::get_system_category());
            return ;
        }
        
        if(::setsockopt(s, SOL_TCP, TCP_KEEPINTVL
            , (void*)&interval, sizeof(interval)) == SOCKET_ERROR)
        {
            ec = boost::system::error_code(errno
                , boost::asio::error::get_system_category());
            return ;
        }

        int default_count = 5;  // 默认最多可允许丢失的心跳包数量
        if(::setsockopt(s, SOL_TCP, TCP_KEEPCNT
            , (void*)&default_count, sizeof(default_count)) == SOCKET_ERROR)
        {
            ec = boost::system::error_code(errno
                , boost::asio::error::get_system_category());
            return ;
        }

        ec.clear();
    }
#endif 

} //namespace pltf

    //////////////////////////////////////////////////////////////////////////
    /// 设置心跳检测
    // @idle: 首次探测开始前的tcp无数据收发空闲时间(单位: 秒)
    // @interval: 每次探测的间隔时间(单位: 秒)
    template <typename Protocol, typename SocketService>
    inline void set_keepalive(basic_socket<Protocol, SocketService> & s
        , int idle, int interval, boost::system::error_code & ec)
    {
        s.set_option(socket_base::keep_alive(true), ec);
        if (ec) return ;
        pltf::set_keepalive_params(s.native_handle(), idle, interval, ec);
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BEXIO_SOCKET_OPTIONS_HPP__