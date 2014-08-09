#ifndef __BEX_UTILITY_FORMAT_HPP__
#define __BEX_UTILITY_FORMAT_HPP__

#include <boost/format.hpp>
#include <boost/preprocessor.hpp>

//////////////////////////////////////////////////////////////////////////
/// 字符串格式化

#ifndef BEX_FORMAT_MAX_PARAMS
#define BEX_FORMAT_MAX_PARAMS 16
#endif //BEX_FORMAT_MAX_PARAMS

namespace Bex
{
    inline std::string format(std::string const& fmt)
    {
        return fmt;
    }

    inline std::wstring format(std::wstring const& fmt)
    {
        return fmt;
    }

#define _PARAM(z, n, data) BOOST_PP_COMMA_IF(n) Arg##n const& arg##n
#define _FMT(z, n, data) % arg##n
#define BOOST_PP_LOCAL_MACRO(n)                                                     \
    template < BOOST_PP_ENUM_PARAMS(n, typename Arg) >                              \
    std::string format(std::string const& fmt, BOOST_PP_REPEAT(n, _PARAM, ~))       \
    {                                                                               \
        try                                                                         \
        {                                                                           \
            return (boost::format(fmt) BOOST_PP_REPEAT(n, _FMT, ~)).str();          \
        }                                                                           \
        catch(std::exception&)                                                      \
        {                                                                           \
            return std::string();                                                   \
        }                                                                           \
    }                                                                               \
                                                                                    \
    template < BOOST_PP_ENUM_PARAMS(n, typename Arg) >                              \
    std::wstring format(std::wstring const& fmt, BOOST_PP_REPEAT(n, _PARAM, ~))     \
    {                                                                               \
        try                                                                         \
        {                                                                           \
            return (boost::wformat(fmt) BOOST_PP_REPEAT(n, _FMT, ~)).str();         \
        }                                                                           \
        catch(std::exception&)                                                      \
        {                                                                           \
            return std::wstring();                                                  \
        }                                                                           \
    }

#define BOOST_PP_LOCAL_LIMITS (1, BEX_FORMAT_MAX_PARAMS)
#include BOOST_PP_LOCAL_ITERATE()
#undef _FMT
#undef _PARAM

} //namespace Bex

#endif //__BEX_UTILITY_FORMAT_HPP__