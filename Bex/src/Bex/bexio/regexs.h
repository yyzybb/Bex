#ifndef __BEX_IO_REGEXS_H__
#define __BEX_IO_REGEXS_H__

#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/algorithm/string.hpp>

namespace Bex {
namespace bexio {

static const char * re_ipv4 = R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})";
static const char * re_ipv6 = R"(
    ::([\dabcdefABCDEF]{1,4}:){0,6}([\dabcdefABCDEF]{1,4})? | [\dabcdefABCDEF]{1,4}::([\dabcdefABCDEF]{1,4}:){0,5}[\dabcdefABCDEF]{1,4} |
    ([\dabcdefABCDEF]{1,4}){2}::([\dabcdefABCDEF]{1,4}:){0,4}[\dabcdefABCDEF]{1,4} |
    ([\dabcdefABCDEF]{1,4}){3}::([\dabcdefABCDEF]{1,4}:){0,3}[\dabcdefABCDEF]{1,4} |
    ([\dabcdefABCDEF]{1,4}){4}::([\dabcdefABCDEF]{1,4}:){0,2}[\dabcdefABCDEF]{1,4} |
    ([\dabcdefABCDEF]{1,4}){5}::([\dabcdefABCDEF]{1,4}:){0,1}[\dabcdefABCDEF]{1,4} |
    ([\dabcdefABCDEF]{1,4}){6}::[\dabcdefABCDEF]{1,4} |
    ([\dabcdefABCDEF]{1,4}:){7}[\dabcdefABCDEF]{1,4} |
    ::([fF]{4}:)?\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}
    )";
static const char * re_httpheader = R"(^([^\s]+)\s+([^\s]+)\s+([^\s]+)\r\n
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    (?:([^\s:]+):\s*((?:[^\r]|\r\n\s+)+)\r\n)?
    \r\n)";

struct regex
{
    static bool is_ipv4(const char * str)
    {
        static auto re = ::boost::xpressive::cregex::compile(re_ipv4);
        return ::boost::xpressive::regex_match(str, re);
    }

    static bool is_ipv6(const char * str)
    {
        static auto re = ::boost::xpressive::cregex::compile(re_ipv6
            , ::boost::xpressive::regex_constants::ignore_white_space);

        return ::boost::xpressive::regex_match(str, re
            , ::boost::xpressive::regex_constants::format_all);
    }

    static bool is_ip(const char * str)
    {
        return is_ipv4(str) || is_ipv6(str);
    }
};

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_REGEXS_H__