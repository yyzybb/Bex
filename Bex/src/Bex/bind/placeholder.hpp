#ifndef __BEX_BIND_PLACEHOLDER_HPP__
#define __BEX_BIND_PLACEHOLDER_HPP__

#include <Bex/bind/forward_bind_fwd.h>

namespace Bex { namespace forward_bind
{
    template <int I>
    struct placeholder {};

    template <typename T>
    struct placeholder_traits
    {
        static const bool is_placeholder = false;
    };

    template <int I>
    struct placeholder_traits<placeholder<I>>
    {
        static const bool is_placeholder = true;
        static const int from_zero_index = I - 1;
    };

} //namespace forward_bind
} //namespace Bex

#endif //__BEX_BIND_PLACEHOLDER_HPP__