#ifndef __BEX_CONFIG_CONFIG_HPP__
#define __BEX_CONFIG_CONFIG_HPP__

#define BEX_LIB_VERSION "1_00"

#include <boost/config.hpp>
#include <boost/system/api_config.hpp>
#include <Bex/config/stl.hpp>
#include <boost/asio/detail/config.hpp>

#if !defined(BOOST_NO_CXX11_DECLTYPE) && !defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) \
    && !defined(BOOST_NO_CXX11_NULLPTR) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) \
    && !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
# define BEX_SUPPORT_CXX11
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1800)
# define BEX_FINAL
#else
# define BEX_FINAL final
#endif 

#if defined(_MSC_VER) && (_MSC_VER < 1500)
# define BEX_OVERRIDE
#else
# define BEX_OVERRIDE override
#endif 

#if defined(BOOST_NO_CXX11_SCOPED_ENUMS)
# define BEX_ENUM_CLASS
#else
# define BEX_ENUM_CLASS class
#endif

#if defined(BOOST_WINDOWS_API)
# define BEX_WINDOWS_API
#endif //defined(BOOST_WINDOWS_API)

#if defined(BOOST_POSIX_API)
# define BEX_POSIX_API
#endif //defined(BOOST_POSIX_API)

#if defined(BOOST_NO_CXX11_CONSTEXPR)
# define BEX_CONSTEXPR const
#else
# define BEX_CONSTEXPR constexpr
#endif //defined(BOOST_NO_CXX11_CONSTEXPR)

# define BEX_MOVE_ARG BOOST_ASIO_MOVE_ARG
# define BEX_MOVE_CAST BOOST_ASIO_MOVE_CAST

#endif //__BEX_CONFIG_CONFIG_HPP__