#ifndef __BEX_PLATFORM_CHARSET_CVT_H__
#define __BEX_PLATFORM_CHARSET_CVT_H__

#ifdef _WIN32
#include "win32/charset_cvt.h"
#endif //_WIN32

#include <string>
#include <Bex/type_traits/class_info.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>

//////////////////////////////////////////////////////////////////////////
/// 字符编码转换
namespace Bex
{
    /// 常用字符集枚举(扩展时只需特化一个相应的charset_traits)
    enum _emCharset
    {
        EC_Utf16,           ///< utf-16 (unicode)
        EC_Local,           ///< 本地默认
        EC_Gbk,             ///< GBK(简体中文windows系统默认字符集)
        EC_Utf8,            ///< utf-8
        EC_Extend_Begin,    ///< 扩展
    };

    /// 字符集特征
    template <int CharsetEnum>
    struct charset_traits;

    static const int invalid_code_page = -1;

    namespace detail
    {
        template <class CharsetTrait>
        struct get_code_page
        {
            inline static const int cp()
            {
                return CharsetTrait::code_page;
            }
        };

        template <class CharsetTrait>
        struct get_charset_name
        {
            inline static const char* name()
            {
                return CharsetTrait::name();
            }
        };

        template <typename CharT = char,
            template <typename> class CharTraits = std::char_traits,
            template <typename> class Allocator = std::allocator >
        struct stl_string_t
        {
            template <typename RebindT>
            struct rebind
            {
                typedef stl_string_t<RebindT, CharTraits, Allocator> type;
            };

            typedef std::basic_string<CharT, CharTraits<CharT>, Allocator<CharT> > type;
        };

        template <class StringType>
        struct string_traits;

        template <typename CharT,
            template <typename> class CharTraits,
            template <typename> class Allocator>
        struct string_traits<std::basic_string<CharT, CharTraits<CharT>, Allocator<CharT> > >
        {
            typedef stl_string_t<CharT, CharTraits, Allocator> type;
            
            template <typename RebindT>
            struct rebind_string_t
            {
                typedef std::basic_string<RebindT, CharTraits<RebindT>, Allocator<RebindT> > type;
            };
        };

        template <int Encode, typename string_t = stl_string_t<> >
        struct charset_cvt_invoker;

        template <int Encode, typename string_t>
        struct charset_cvt_invoker
        {
            typedef charset_traits<Encode> CharsetTrait;

            typedef typename string_t::template rebind<char>::type::type string_type;
            typedef typename string_t::template rebind<wchar_t>::type::type wstring_type;

            inline wstring_type to_utf16(string_type const& str) const
            {
                return ctow<wstring_type>(str, CharsetTrait::code_page, CharsetTrait::name());
            }

            inline string_type from_utf16(wstring_type const& wstr) const
            {
                return wtoc<string_type>(wstr, CharsetTrait::code_page, CharsetTrait::name());
            }
        };

        template <typename string_t>
        struct charset_cvt_invoker<EC_Utf16, string_t>
        {
            typedef typename string_t::template rebind<wchar_t>::type::type string_type;
            typedef typename string_t::template rebind<wchar_t>::type::type wstring_type;

            inline wstring_type to_utf16(string_type const& str) const
            {
                return str;
            }

            inline string_type from_utf16(wstring_type const& wstr) const
            {
                return wstr;
            }
        };
    } //namespace detail

    using detail::charset_cvt_invoker;
    using detail::string_traits;

    template <>
    struct charset_traits<EC_Local>
    {
        static const int code_page = 0;
        inline static const char* name()
        {
            return "local";
        }
    };

    template <>
    struct charset_traits<EC_Gbk>
    {
        static const int code_page = 936;
        inline static const char* name()
        {
            return "gbk";
        }
    };

    template <>
    struct charset_traits<EC_Utf8>
    {
        static const int code_page = 65001;
        inline static const char* name()
        {
            return "utf-8";
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// 字符集转换自由函数
    template <int SrcCharsetEnum, int DstCharsetEnum, class string_t>
    typename charset_cvt_invoker<DstCharsetEnum, typename string_traits<string_t>::type>::string_type 
        charset_cvt(string_t const& str)
    {
        return charset_cvt_invoker<DstCharsetEnum, typename string_traits<string_t>::type>().from_utf16(
            charset_cvt_invoker<SrcCharsetEnum, typename string_traits<string_t>::type>().to_utf16(str));
    }

    // ansi -> unicode
    template <class string_t>
    inline typename string_traits<string_t>::template rebind_string_t<wchar_t>::type
        a2w(string_t const& str)
    {
        BOOST_STATIC_ASSERT((boost::is_same<char, typename string_t::value_type>::value));
        return charset_cvt<EC_Local, EC_Utf16>(str);
    }

    // unicode -> ansi
    template <class string_t>
    inline typename string_traits<string_t>::template rebind_string_t<char>::type
        w2a(string_t const& wstr)
    {
        BOOST_STATIC_ASSERT((boost::is_same<wchar_t, typename string_t::value_type>::value));
        return charset_cvt<EC_Utf16, EC_Local>(wstr);
    }
    
    // ansi -> utf8
    template <class string_t>
    inline typename string_traits<string_t>::template rebind_string_t<char>::type
        a2u8(string_t const& str)
    {
        BOOST_STATIC_ASSERT((boost::is_same<char, typename string_t::value_type>::value));
        return charset_cvt<EC_Local, EC_Utf8>(str);
    }

    // utf8 -> ansi
    template <class string_t>
    inline typename string_traits<string_t>::template rebind_string_t<char>::type
        u82a(string_t const& str)
    {
        BOOST_STATIC_ASSERT((boost::is_same<char, typename string_t::value_type>::value));
        return charset_cvt<EC_Utf8, EC_Local>(str);
    }

    // utf8 -> unicode
    template <class string_t>
    inline typename string_traits<string_t>::template rebind_string_t<wchar_t>::type
        u82w(string_t const& str)
    {
        BOOST_STATIC_ASSERT((boost::is_same<char, typename string_t::value_type>::value));
        return charset_cvt<EC_Utf8, EC_Utf16>(str);
    }

    // unicode -> utf8
    template <class string_t>
    inline typename string_traits<string_t>::template rebind_string_t<char>::type
        w2u8(string_t const& wstr)
    {
        BOOST_STATIC_ASSERT((boost::is_same<wchar_t, typename string_t::value_type>::value));
        return charset_cvt<EC_Utf16, EC_Utf8>(wstr);
    }

} //namespace Bex

#endif //__BEX_PLATFORM_CHARSET_CVT_H__