#include <Bex/bind/forward_bind.hpp>

namespace Bex { 
#define BOOST_PP_LOCAL_MACRO(n) \
    forward_bind::placeholder<n> _##n;

#define BOOST_PP_LOCAL_LIMITS (1, BEX_FORWARD_BIND_PARAMETER_COUNT)
#include BOOST_PP_LOCAL_ITERATE()
}