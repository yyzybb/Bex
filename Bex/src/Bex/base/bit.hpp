#ifndef __BEX_BIT__
#define __BEX_BIT__

#include "type_defines.hpp"
#include <boost/static_assert.hpp>

//////////////////////////////////////////////////////////////////////////
/// ------ BIT²Ù×÷

namespace Bex
{
    /// ------ least bit integer
    template <int Bit>
    struct least_bv
    {
        typedef typename boost::uint_t<Bit + 1>::least type;
        static const type value = (type)1 << Bit;
    };

    /// ------ get strong
    template <typename L, typename R>
    struct stronger_integer
    {
        BOOST_STATIC_ASSERT(std::is_integral<L>::value);
        BOOST_STATIC_ASSERT(std::is_integral<R>::value);

        typedef typename boost::mpl::if_c<
            (sizeof(L) > sizeof(R)),
            L, R > ::type type;
    };

    /// ------ make bit integer
    template <typename Integer>
    inline Integer make_bv(int bit)
    {
        return (Integer)1 << bit;
    }

    /// ------ test bit
    template <typename L, typename R>
    inline bool test_bit_value(L const& lhs, R const& rhs)
    {
        BOOST_STATIC_ASSERT((std::is_integral<L>::value && std::is_integral<R>::value));

        typedef typename std::make_unsigned<L>::type UL;
        typedef typename std::make_unsigned<R>::type UR;
        typedef typename stronger_integer<UL, UR>::type strong_type;

        return static_cast<bool>(((strong_type)(UL)lhs & (strong_type)(UR)rhs) != 0);
    }

    template <typename T>
    inline bool test_bit(T const& t, int bit)
    {
        BOOST_STATIC_ASSERT((std::is_integral<T>::value));
        if (bit >= sizeof(T)* 8) return false;
        return test_bit_value(t, ((T)1 << bit));
    }

    /// ------ set bit
    template <typename L, typename R>
    inline L set_bit_value(L const& lhs, R const& rhs, bool b)
    {
        BOOST_STATIC_ASSERT((std::is_integral<L>::value && std::is_integral<R>::value));

        typedef typename std::make_unsigned<L>::type UL;
        typedef typename std::make_unsigned<R>::type UR;
        typedef typename stronger_integer<UL, UR>::type strong_type;

        if (b)
            return (L)(UL)((strong_type)(UL)lhs | (strong_type)(UR)rhs);
        else
            return (L)(UL)((strong_type)(UL)lhs & ~(strong_type)(UR)rhs);
    }

    template <typename T>
    inline T set_bit(T const& t, int bit, bool b)
    {
        BOOST_STATIC_ASSERT((std::is_integral<T>::value));
        if (bit >= sizeof(T)* 8) return t;
        return set_bit_value(t, ((T)1 << bit), b);
    }


#if !defined(BEX_NO_BIT_MACRO)

# define BEX_BIT(x) (Bex::make_bv<uint64_t>(x))
# define BEX_BIT32(x) (Bex::make_bv<uint32_t>(x))
# define BEX_LEAST_BIT(constexpr_bit) (Bex::least_bv<constexpr_bit>::value)

# define BEX_TEST_BIT_VALUE(value, bv) (Bex::test_bit_value(value, bv))
# define BEX_TEST_BIT(value, bit) (Bex::test_bit(value, bit))

# define BEX_SET_BIT_VALUE(value, bv, _b) (value = Bex::set_bit_value(value, bv, _b))
# define BEX_SET_BIT(value, bit, _b) (value = Bex::set_bit(value, bit, _b))
        
#endif //BEX_NO_BIT_MACRO
}


#endif //__BEX_BIT__