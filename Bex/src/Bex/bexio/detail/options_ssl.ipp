#ifndef __BEX_IO_DETAIL_OPTIONS_SSL_IPP__
#define __BEX_IO_DETAIL_OPTIONS_SSL_IPP__

#include <boost/asio/ssl/context_base.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <Bex/bexio/options_ssl.hpp>

namespace Bex { namespace bexio 
{
    // 客户端默认配置
    ssl_options ssl_options::client()
    {
        static ssl_options opts = {
            ssl::context_base::default_workarounds | ssl::context_base::no_sslv2,
            ssl::verify_none,
            "",
            "",
            ssl::context_base::pem,
            "dh512.pem",
            "RC4-MD5",
            10 * 1000
        };
        return opts;
    }

    // 服务端默认配置
    ssl_options ssl_options::server()
    {
        static ssl_options opts = {
            ssl::context_base::default_workarounds | ssl::context_base::no_sslv2,
            ssl::verify_none,
            "bexio.crt",
            "bexio.pem",
            ssl::context_base::pem,
            "dh512.pem",
            "RC4-MD5",
            10 * 1000
        };
        return opts;
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_DETAIL_OPTIONS_SSL_IPP__