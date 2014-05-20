#include "charset_cvt.h"
#include <Bex/platform/charset_cvt.h>
#include <boost/assert.hpp>
#include <Windows.h>

namespace Bex
{
    // 多字节字符集 -> unicode
    int ctow_api(int code_page, char const* data, int length, wchar_t * output, int output_length)
    {
        BOOST_ASSERT(code_page != invalid_code_page);
        BOOST_ASSERT(code_page >= 0);
        BOOST_ASSERT((CP_ACP == code_page || ::IsValidCodePage(code_page)));
        return ::MultiByteToWideChar(code_page, 0, data, length, output, output_length);
    }

    // unicode -> 多字节字符集
    int wtoc_api(int code_page, wchar_t const* data, int length, char * output, int output_length)
    {
        BOOST_ASSERT(code_page != invalid_code_page);
        BOOST_ASSERT(code_page >= 0);
        BOOST_ASSERT((CP_ACP == code_page || ::IsValidCodePage(code_page)));
        return ::WideCharToMultiByte(code_page, 0, data, length, output, output_length, NULL, NULL);
    }

} //namespace Bex