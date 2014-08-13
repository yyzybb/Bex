#ifndef __BEX_IO_ERROR_HPP__
#define __BEX_IO_ERROR_HPP__

//////////////////////////////////////////////////////////////////////////
/// 自定义errorcode
#include <Bex/config.hpp>
#include <boost/system/error_code.hpp>

namespace Bex { namespace bexio
{
    enum BEX_ENUM_CLASS bexio_error_em
    {
        /// ------ sessions errors
        initiative_terminate,   ///< 强制主动断开连接
        initiative_shutdown,    ///< 优雅地主动断开连接
        passive_shutdown,       ///< 优雅地被动断开连接
        sendbuffer_overflow,    ///< 发送缓冲区溢出
        receivebuffer_overflow, ///< 接收缓冲区溢出
        connect_overtime,       ///< 连接超时
        parse_error,            ///< 数据解析出错
        reconnect_error,        ///< 重连失败
        miss_ssl_options,       ///< SSL选项未设置
        handshake_overtime,     ///< SSL握手超时
        shutdown_overtime,      ///< 优雅地关闭连接超时

        /// ------ url parse errors
        url_miss_protocol_split,        ///< url的protocol后面缺少"://"分隔符
        url_miss_match_brackets,        ///< '['与']'不匹配
        url_ipv6_host_toomore,          ///< ipv6的host不需要更多额外信息
        url_miss_port_after_risk,       ///< ':'后面缺少端口号
        url_port_mustbe_digit,          ///< 端口号必须为数字
        url_path_contains_invaild_char, ///< path中含有非法字符

        /// ------ http errors
        http_response_header_parse_error,   ///< response头解析失败
    };
    typedef bexio_error_em errc;

#define BEX_IO_CASE_ERRORCODE(c) case (int)errc::c: return #c

    class bexio_error
        : public boost::system::error_category
    {
#if defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
        bexio_error() {}
#else  // defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
        bexio_error() = default;
#endif // defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)

    public:
        virtual char const* name() const BOOST_SYSTEM_NOEXCEPT
        {
            return "bexio_error";
        }

        virtual std::string message(int ev) const BOOST_SYSTEM_NOEXCEPT
        {
            switch(ev)
            {
            // session errors
            BEX_IO_CASE_ERRORCODE(initiative_terminate);
            BEX_IO_CASE_ERRORCODE(initiative_shutdown);
            BEX_IO_CASE_ERRORCODE(passive_shutdown);
            BEX_IO_CASE_ERRORCODE(sendbuffer_overflow);
            BEX_IO_CASE_ERRORCODE(receivebuffer_overflow);
            BEX_IO_CASE_ERRORCODE(connect_overtime);
            BEX_IO_CASE_ERRORCODE(parse_error);
            BEX_IO_CASE_ERRORCODE(reconnect_error);
            BEX_IO_CASE_ERRORCODE(miss_ssl_options);
            BEX_IO_CASE_ERRORCODE(handshake_overtime);
            BEX_IO_CASE_ERRORCODE(shutdown_overtime);

            // url parse errors
            BEX_IO_CASE_ERRORCODE(url_miss_protocol_split);
            BEX_IO_CASE_ERRORCODE(url_miss_match_brackets);
            BEX_IO_CASE_ERRORCODE(url_ipv6_host_toomore);
            BEX_IO_CASE_ERRORCODE(url_miss_port_after_risk);
            BEX_IO_CASE_ERRORCODE(url_port_mustbe_digit);
            BEX_IO_CASE_ERRORCODE(url_path_contains_invaild_char);

            // http errors
            BEX_IO_CASE_ERRORCODE(http_response_header_parse_error);

            default:
                return "undefined bexio error";
            }
        }

        friend bexio_error const& get_bexio_category();
    };

    inline bexio_error const& get_bexio_category()
    {
        static bexio_error err;
        return err;
    }

    /// 生成bexio_error类型的error_code
    inline boost::system::error_code make_error_code(bexio_error_em ev)
    {
        return boost::system::error_code((int)ev, get_bexio_category());
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_ERROR_HPP__