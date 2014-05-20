#include "TestPCH.h"
using namespace Bex;

BOOST_AUTO_TEST_SUITE(s_test_base)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_test_bit)
{
    XDump(" 开始测试 bit.hpp ");

    /// ------ BEX_LEAST_BIT
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(0), 1);
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(1), 2);
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(2), 4);
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(7), 0x80);
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(15), ((uint16_t)1 << 15));
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(31), ((uint32_t)1 << 31));
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(32), ((uint64_t)1 << 32));
    BOOST_CHECK_EQUAL(BEX_LEAST_BIT(63), ((uint64_t)1 << 63));

    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(0)), 1);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(7)), 1);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(8)), 2);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(15)), 2);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(16)), 4);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(31)), 4);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(32)), 8);
    BOOST_CHECK_EQUAL(sizeof(BEX_LEAST_BIT(62)), 8);

    /// ------ BIT
    FOR(64)
    {
        BOOST_CHECK_EQUAL(BEX_BIT(i), ((uint64_t)1 << i));
    }

    /// ------ SET_BIT_MARK SET_BIT_VALUE
    uint8_t flag8 = 0;
    uint16_t flag16 = 0;
    uint32_t flag32 = 0;
    uint64_t flag64 = 0;

    FOR(8)
    {
        BEX_SET_BIT(flag8, i, true);
    }
    BOOST_CHECK_EQUAL(flag8, (std::numeric_limits<uint8_t>::max)());

    FOR(64)
    {
        BEX_SET_BIT(flag64, i, true);
    }
    BOOST_CHECK_EQUAL(flag64, (std::numeric_limits<uint64_t>::max)());

    BEX_SET_BIT(flag32, 31, true);
    BOOST_CHECK_EQUAL(flag32, BEX_LEAST_BIT(31));

    /// ------ TEST_BIT_MARK TEST_BIT_VALUE
    FOR(8)
    {
        BOOST_CHECK(BEX_TEST_BIT(flag8, i));
        BOOST_CHECK(BEX_TEST_BIT_VALUE(flag8, BEX_BIT(i)));
    }

    FOR(64)
    {
        BOOST_CHECK(BEX_TEST_BIT(flag64, i));
        BOOST_CHECK(BEX_TEST_BIT_VALUE(flag64, BEX_BIT(i)));
    }

    BOOST_CHECK(BEX_TEST_BIT(0x3, 0));
    BOOST_CHECK(BEX_TEST_BIT(0x3, 1));
    BOOST_CHECK(!BEX_TEST_BIT(0x3, 2));

    BOOST_CHECK(BEX_TEST_BIT_VALUE(0x3, 0x1));
    BOOST_CHECK(BEX_TEST_BIT_VALUE(0x3, 0x2));
    BOOST_CHECK(BEX_TEST_BIT_VALUE(0x3f, 0x0f));
    BOOST_CHECK(BEX_TEST_BIT_VALUE(0x3f, 0x07));
    BOOST_CHECK(BEX_TEST_BIT_VALUE(0x3f, 0x30));

    XDump(" 结束测试 bit.hpp ");
}

BOOST_AUTO_TEST_SUITE_END()