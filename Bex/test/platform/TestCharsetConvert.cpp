#include "TestPCH.h"
#include <Bex/locale/charset_cvt.h>
using namespace Bex;
using namespace Bex::conv;

BOOST_AUTO_TEST_SUITE(s_charset_cvt)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_charset_cvt)
{
    XDump("开始测试 charset_cvt");

    std::string str = u82a("我们1234a");

    std::string u8 = a2u8(str);
    std::string u8_check = u82a(u8);
    BOOST_CHECK(str == u8_check);

    //std::string big5 = between(u8, "big5", "utf8");    
    //std::string big5_check = between(big5, "utf8", "big5");
    //BOOST_CHECK(u8 == big5_check);

    std::string gbk = between(u8, "gbk", "utf8");    
    std::string gbk_check = between(gbk, "utf8", "gbk");
    BOOST_CHECK(u8 == gbk_check);

    std::wstring u16 = a2w(str);
    std::string u16_check = w2a(u16);
    BOOST_CHECK(str == u16_check);

    std::wstring tmp = u82w(u8);
    u8_check = w2u8(tmp);
    BOOST_CHECK(u8 == u8_check);

    XDump("结束测试 charset_cvt");
}

BOOST_AUTO_TEST_SUITE_END()