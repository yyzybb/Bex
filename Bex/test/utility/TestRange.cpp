#include "TestPCH.h"
#include <Bex/utility/range.hpp>

#if defined(BEX_SUPPORT_CXX11)

BOOST_AUTO_TEST_SUITE(s_range)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_range)
{
    XDump("开始测试 range");

    int v = 0;
    for (auto i : range(5))
        v += i;
    BOOST_CHECK(v == (0 + 1 + 2 + 3 + 4));

    v = 0;
    for (auto i : range(1, 3))
        v += i;
    BOOST_CHECK(v == (1 + 2));

    XDump("结束测试 range");
}

BOOST_AUTO_TEST_SUITE_END()

#endif //defined(BEX_SUPPORT_CXX11)