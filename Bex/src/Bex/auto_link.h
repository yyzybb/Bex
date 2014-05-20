#ifndef __BEX_AUTO_LINK_H__
#define __BEX_AUTO_LINK_H__

#include <Bex/config.hpp>

#define BOOST_LIB_NAME "Bex"

#if defined(BOOST_LIB_VERSION)
#
# pragma push_macro("BOOST_LIB_VERSION")
#  undef BOOST_LIB_VERSION
#  define BOOST_LIB_VERSION BEX_LIB_VERSION
#  include <boost/config/auto_link.hpp>
# pragma pop_macro("BOOST_LIB_VERSION")
#
#else  //defined(BOOST_LIB_VERSION)
#
# define BOOST_LIB_VERSION BEX_LIB_VERSION
# include <boost/config/auto_link.hpp>
# undef BOOST_LIB_VERSION
#
#endif //defined(BOOST_LIB_VERSION)

#endif //__BEX_AUTO_LINK_H__