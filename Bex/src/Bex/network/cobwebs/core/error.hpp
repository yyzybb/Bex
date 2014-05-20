#ifndef __BEX_NETWORK_COBWEBS_ERROR_HPP__
#define __BEX_NETWORK_COBWEBS_ERROR_HPP__

//////////////////////////////////////////////////////////////////////////
/// 自定义错误信息

#include <boost/system/error_code.hpp>
#include <Bex/utility/singleton.hpp>

namespace Bex { namespace cobwebs
{
    enum _emCobwebsErrorCode
    {
        cec_undefined,              ///< 未定义错误
        cec_sendbuf_overflow,       ///< 发送缓冲区溢出
        cec_recvbuf_overflow,       ///< 接收缓冲区溢出
        cec_driving_shutdown,       ///< 主动优雅地关闭
        cec_driving_close,          ///< 主动强制关闭
    };

    class cobwebs_error_category
        : virtual public boost::system::error_category
        , virtual public singleton<cobwebs_error_category>
    {
    public:
        virtual const char *     name() const throw()/*BOOST_SYSTEM_NOEXCEPT*/
        {
            return "cobwebs error";
        }

        virtual std::string      message( int ev ) const
        {
            switch (ev)
            {
            case cec_recvbuf_overflow:
                return "recv buf overflow";

            case cec_sendbuf_overflow:
                return "send buf overflow";

            case cec_driving_shutdown:
                return "driving shutdown";

            case cec_driving_close:
                return "driving close";

            case cec_undefined:
            default:
                return "undefined cobwebs error";
            }
        }
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_ERROR_HPP__