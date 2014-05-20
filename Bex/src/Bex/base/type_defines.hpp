#ifndef __BEX_TYPE_DEFINES__
#define __BEX_TYPE_DEFINES__

#include <type_traits>
#include <boost/cstdint.hpp>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

//////////////////////////////////////////////////////////////////////////
/// ------ 类型定义

namespace Bex
{
    /// ------ Define integer types
    using boost::int8_t;
    using boost::uint8_t;

    using boost::int16_t;
    using boost::uint16_t;

    using boost::int32_t;
    using boost::uint32_t;

    using boost::int64_t;
    using boost::uint64_t;

    typedef float float32_t;
    typedef double float64_t;

    /// ------ Make integer type from bytes or bits.
    template <int Bytes>
    struct integer_type;

    template <int Bits>
    struct integer_type_b;

    namespace detail
    {
        template <int Bytes>
        struct integer_type_helper;

        template <>
        struct integer_type_helper<1>
        {
            typedef int8_t signed_type;
            typedef uint8_t unsigned_type;
        };

        template <>
        struct integer_type_helper<2>
        {
            typedef int16_t signed_type;
            typedef uint16_t unsigned_type;
        };

        template <>
        struct integer_type_helper<4>
        {
            typedef int32_t signed_type;
            typedef uint32_t unsigned_type;
        };

        template <>
        struct integer_type_helper<8>
        {
            typedef int64_t signed_type;
            typedef uint64_t unsigned_type;
        };
    }

    template <int Bytes>
    struct integer_type
        : detail::integer_type_helper<
        (Bytes <= 1 ? 1 :
          (Bytes <= 2 ? 2 :
            (Bytes <= 4 ? 4 :
              (Bytes <= 8 ? 8 : Bytes))))
        >
    {
    };

    template <int Bits>
    struct integer_type_b
        : integer_type< (Bits / 8 + (Bits % 8 ? 1 : 0)) >
    {
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

} //namespace Bex

#endif //__BEX_TYPE_DEFINES__