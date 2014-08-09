#ifndef __BEX_MATH_COMPRESS_NUMERIC__
#define __BEX_MATH_COMPRESS_NUMERIC__

//////////////////////////////////////////////////////////////////////////
/// 压缩存储的整数类型

#include <Bex/base.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/if.hpp>
#include "utility.hpp"

namespace Bex
{
    template <int N>
    class basic_compress_numeric
    {
        BOOST_STATIC_ASSERT(N >= 1 && N <= 8);

    public:
        enum {
            size = N,
            bit_count = 7,
            elem_max_value = (1 << bit_count),
            max_value = static_pow<elem_max_value, size>::value - 1,
        };

        typedef typename boost::mpl::if_c<(size <= 4), uint32_t, uint64_t>::type Integer;

    private:
        uint8_t m_value[size];

    public:
        basic_compress_numeric()
        {
            m_value[0] = 0;
        }

        explicit basic_compress_numeric(Integer value)
        {
            set(value);
        }

        void set(Integer value)
        {
            for (int i = 0; i < size; ++i)
            {
                m_value[i] = (uint8_t)(value & (elem_max_value - 1));
                value = value >> bit_count;

                if (value)
                    m_value[i] |= elem_max_value;
                else
                    break;
            }
        }

        Integer get() const
        {
            Integer value = 0;
            for (int i = 0; i < size; ++i)
            {
                value += ((Integer)(m_value[i] & (~elem_max_value)) << (bit_count * i));
                if (!(m_value[i] & elem_max_value))
                    break;
            }

            return value;
        }

        bool is_overflow() const
        {
            for (int i = 0; i < size; ++i)
                if (!(m_value[i] & elem_max_value))
                    return false;

            return true;
        }

        template <class Archive>
        void serialize(Archive & ar, const unsigned int /*version*/)
        {
            ar & m_value[0];
            for (int i = 1; i < size; ++i)
            {
                if (m_value[i-1] & elem_max_value)
                    ar & m_value[i];
                else
                    break;
            }
        }
    };

    typedef basic_compress_numeric<1> cn8;
    typedef basic_compress_numeric<2> cn16;
    typedef basic_compress_numeric<4> cn32;
    typedef basic_compress_numeric<8> cn64;

} //namespace Bex

#endif //__BEX_MATH_COMPRESS_NUMERIC__