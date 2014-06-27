#ifndef __BEX_PLATFORM_WIN32_CHARSET_CVT_H__
#define __BEX_PLATFORM_WIN32_CHARSET_CVT_H__

#if defined(_WIN32)

#include <string>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

//////////////////////////////////////////////////////////////////////////
/// ×Ö·û±àÂë×ª»»
namespace Bex
{
    // ¶à×Ö½Ú×Ö·û¼¯ -> unicode
    int ctow_api(int code_page, char const* data, int length, wchar_t * output, int output_length);

    template <class wstring_t, class string_t>
    inline wstring_t ctow(string_t const& str, int code_page, const char*)
    {
        BOOST_STATIC_ASSERT((boost::is_same<char, typename string_t::value_type>::value));
        BOOST_STATIC_ASSERT((boost::is_same<wchar_t, typename wstring_t::value_type>::value));

        wstring_t out;
        int capacity = ctow_api(code_page, str.c_str(), str.length(), NULL, 0);
        if (capacity > 0)
        {
            out.resize(capacity);
            ctow_api(code_page, str.c_str(), str.length(), &out[0], out.length());
        }
        return out;
    }

    // unicode -> ¶à×Ö½Ú×Ö·û¼¯
    int wtoc_api(int code_page, wchar_t const* data, int length, char * output, int output_length);

    template <class string_t, class wstring_t>
    string_t wtoc(wstring_t const& wstr, int code_page, const char*)
    {
        BOOST_STATIC_ASSERT((boost::is_same<char, typename string_t::value_type>::value));
        BOOST_STATIC_ASSERT((boost::is_same<wchar_t, typename wstring_t::value_type>::value));

        string_t out;
        int capacity = wtoc_api(code_page, wstr.c_str(), wstr.length(), 0, 0);
        if (capacity > 0)
        {
            out.resize(capacity);
            wtoc_api(code_page, wstr.c_str(), wstr.length(), &out[0], out.length());
        }
        return out;
    }

} //namespace Bex

#endif //__BEX_PLATFORM_WIN32_CHARSET_CVT_H__