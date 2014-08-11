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
    };
    typedef bexio_error_em bee;

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
            case (int)bee::initiative_terminate:
                return "initiative terminate";

            case (int)bee::initiative_shutdown:
                return "initiative shutdown";

            case (int)bee::passive_shutdown:
                return "passive shutdown";

            case (int)bee::sendbuffer_overflow:
                return "sendbuffer overflow";

            case (int)bee::receivebuffer_overflow:
                return "receivebuffer overflow";

            case (int)bee::connect_overtime:
                return "connect overtime";

            case (int)bee::parse_error:
                return "data parse error";

            case (int)bee::reconnect_error:
                return "reconnect error";

            case (int)bee::miss_ssl_options:
                return "miss ssl options";

            case (int)bee::handshake_overtime:
                return "handshake overtime";

            case (int)bee::shutdown_overtime:
                return "shutdown overtime";

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
    inline boost::system::error_code generate_error(bexio_error_em ev)
    {
        return boost::system::error_code((int)ev, get_bexio_category());
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_ERROR_HPP__