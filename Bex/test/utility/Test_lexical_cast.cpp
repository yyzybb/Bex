#include "TestPCH.h"
#include <Bex/utility/lexical_cast.hpp>
using namespace Bex;

BOOST_AUTO_TEST_SUITE(s_lexical_cast)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_lexical_cast)
{
    XDump("开始测试 lexical_cast");

    int x = lexical_cast_noexcept<int>(std::string("abc"), 1);
    BOOST_CHECK_EQUAL(x, 1);

    x = lexical_cast_noexcept_d<int>(std::string("abc"));
    BOOST_CHECK_EQUAL(x, 0);

    x = lexical_cast_noexcept<int>("abc", 3, 2);
    BOOST_CHECK_EQUAL(x, 2);

    x = lexical_cast_noexcept_d<int>("abc", 3);
    BOOST_CHECK_EQUAL(x, 0);

    XDump("结束测试 lexical_cast");
}

BOOST_AUTO_TEST_SUITE_END()