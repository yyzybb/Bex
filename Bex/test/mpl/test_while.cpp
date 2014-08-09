#include "TestPCH.h"
#include <Bex/mpl/while.hpp>
#include <Bex/utility/type_name.hpp>
#include <boost/type_traits.hpp>

BOOST_AUTO_TEST_SUITE(s_mpl)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_mpl_while)
{
    XDump("开始测试 mpl_while");

    /// compiling test.
    typedef int T[1][2][3][4];
    typedef Bex::mpl::while_<boost::is_array, boost::remove_extent, T>::type result;
    BOOST_STATIC_ASSERT((boost::is_same<int, result>::value));

    XDump("结束测试 mpl_while");
}

BOOST_AUTO_TEST_SUITE_END()