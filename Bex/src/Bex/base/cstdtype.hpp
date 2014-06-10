#ifndef __BEX_TYPE_DEFINES__
#define __BEX_TYPE_DEFINES__

#include <boost/cstdint.hpp>
#include <boost/integer.hpp>
#include <type_traits>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

//////////////////////////////////////////////////////////////////////////
/// ------ 类型定义

namespace Bex {
    namespace cstdint
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
    } //namespace cstdint

    using namespace cstdint;
} //namespace Bex

#endif //__BEX_TYPE_DEFINES__