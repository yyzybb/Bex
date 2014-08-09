#ifndef __BEX_FUNCTOR_FUNCTOR_HPP__
#define __BEX_FUNCTOR_FUNCTOR_HPP__

//////////////////////////////////////////////////////////////////////////
// functors, improve cross platform stl and boost.
#include <Bex/config.hpp>
#include <algorithm>

namespace Bex
{
    // equal_to
    template <typename T = void>
    struct equal_to;

    template <>
    struct equal_to<void>
    {
        template <typename T>
        inline bool operator()(T const& lhs, T const& rhs) const
        {
            return (lhs == rhs);
        }
    };

    template <typename T>
    struct equal_to
    {
        inline bool operator()(T const& lhs, T const& rhs) const
        {
            return (lhs == rhs);
        }
    };

} //namespace Bex

#endif //__BEX_FUNCTOR_FUNCTOR_HPP__