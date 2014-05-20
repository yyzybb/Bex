#ifndef __BEX_CONFIG_CONFIG_HPP__
#define __BEX_CONFIG_CONFIG_HPP__

#define BEX_LIB_VERSION "1_00"

#include <boost/config.hpp>
#include <Bex/config/stl.hpp>

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

#endif //__BEX_CONFIG_CONFIG_HPP__