#include "TestPCH.h"
#include <Bex/type_traits/remove_all.hpp>
using namespace Bex;

class A
{

};

BOOST_AUTO_TEST_SUITE(s_remove_all)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_remove_all)
{
    XDump("开始测试 remove_all");

    //////////////////////////////////////////////////////////////////////////
    /// 单个修饰
    typedef int T;
    typedef int T1;
    typedef int* T2;
    typedef int& T3;
    typedef int const T4;
    typedef int volatile T5;
    typedef int T6[5];

    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T1>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T2>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T3>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T4>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T5>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T6>::type, T>::value ));

    //////////////////////////////////////////////////////////////////////////
    /// 多个修饰
    typedef int * volatile const& T11;
    typedef volatile const int *& T12;
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T11>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T12>::type, T>::value ));

    //////////////////////////////////////////////////////////////////////////
    /// 多层嵌套修饰
    typedef int ** T21;
    typedef int **& T22;
    typedef int const * const T23;
    typedef int const * const & T24;
    typedef int volatile * const & T25;
    typedef int volatile * const T26[5];

    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T21>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T22>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T23>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T24>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T25>::type, T>::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<remove_all<T26>::type, T>::value ));

    XDump("结束测试 remove_all");
}

BOOST_AUTO_TEST_SUITE_END()