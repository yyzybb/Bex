#include "TestPCH.h"
#include <Bex/type_traits/signature_traits.hpp>
using namespace Bex;

class A
{

};

BOOST_AUTO_TEST_SUITE(s_signature_traits)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_signature_traits)
{
    XDump("开始测试 signature_traits");

    {
        typedef signature_traits<void()> st;
        DumpX(typeid(st::result_type).name());
        DumpX(typeid(st::function).name());
        DumpX(typeid(st::member_function<A>::type).name());
    }

    {
        typedef void(*FT)(void);
        typedef signature_traits<FT> st;
        DumpX(typeid(st::result_type).name());
        DumpX(typeid(st::function).name());
        DumpX(typeid(st::member_function<A>::type).name());
    }

    {
        typedef signature_traits<int(double, float)> st;
        DumpX(typeid(st::result_type).name());
        DumpX(typeid(st::function).name());
        DumpX(typeid(st::member_function<A>::type).name());
    }

    XDump("结束测试 signature_traits");
}

BOOST_AUTO_TEST_SUITE_END()