#include "TestPCH.h"
#include <Bex/math/compress_numeric.hpp>
#pragma warning(push)
# pragma warning(disable:4996 4244)
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma warning(pop)

using namespace Bex;


BOOST_AUTO_TEST_SUITE(s_math)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_math)
{
    XDump("开始测试 math.CompressNumeric");

    cn32 cn;
    BOOST_CHECK_EQUAL(cn.get(), 0);

    cn.set(cn32::max_value + 1);
    BOOST_CHECK(cn.is_overflow());

    
    {
        boost::progress_display pd(cn32::max_value);
        for (int i = 0; i <= cn32::max_value; ++i)
        {
            cn.set(i);
            BOOST_CHECK_EQUAL(cn.get(), i);
            BOOST_CHECK(!cn.is_overflow());
            ++ pd;
        }
    }

    {
        boost::progress_display pd(cn32::max_value);
        for (int i = 0; i <= cn32::max_value; ++i)
        {
            cn.set(i);
            BOOST_CHECK_EQUAL(cn.get(), i);
            BOOST_CHECK(!cn.is_overflow());
            ++ pd;
        }
    }

    XDump("结束测试 math.CompressNumeric");
}

BOOST_AUTO_TEST_SUITE_END()