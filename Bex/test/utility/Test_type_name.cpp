#include "TestPCH.h"
#include <Bex/utility/type_name.hpp>
using namespace Bex;

struct X;

BOOST_AUTO_TEST_SUITE(s_type_name)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_type_name)
{
    XDump("开始测试 type_name");

    std::string icr = real_typename<int const&>();
    std::string ia = real_typename<int[4]>();
    std::string iaa = real_typename<int[1][2]>();
    std::string is_c = real_typename<std::is_const<int> >();
    std::string vf = real_typename<void()>();
    std::string vfp = real_typename<void(*)()>();
    std::string vfcp = real_typename<void(X::*)()>();
    std::string ipa = real_typename<int * const[4]>();

#if defined(__GNUC__)
    BOOST_CHECK(icr == "int const&");
    BOOST_CHECK(ia == "int [4]");
    BOOST_CHECK(iaa == "int [1][2]");
    BOOST_CHECK(is_c == "std::is_const<int> ");
    BOOST_CHECK(vf == "void ()");
    BOOST_CHECK(vfp == "void (*)()");
    BOOST_CHECK(vfcp == "void (X::*)()");
#else
    BOOST_CHECK(icr == "int const &");
    BOOST_CHECK(ia == "int [4]");
    BOOST_CHECK(iaa == "int [1][2]");
    BOOST_CHECK(is_c == "struct std::is_const<int> ");
    BOOST_CHECK(vf == "void __cdecl(void)");
    BOOST_CHECK(vfp == "void (__cdecl*)(void)");
    BOOST_CHECK(vfcp == "void (__thiscall X::*)(void)");
#endif

    XDump("结束测试 type_name");
}

BOOST_AUTO_TEST_SUITE_END()