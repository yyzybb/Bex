#ifndef __BEX_MATH_UTILITY__
#define __BEX_MATH_UTILITY__

namespace Bex
{
    /// 乘方计算
    template <int X, int N = 2>
    struct static_pow
    {
        enum {
            value = X * static_pow<X, N-1>::value,
        };
    };
    template <int X>
    struct static_pow<X, 0>
    {
        enum {
            value = 1,
        };
    };
}

#endif //__BEX_MATH_UTILITY__