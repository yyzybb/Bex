#ifndef __BEX_PLATFORM_CHARSET_CVT_H__
#define __BEX_PLATFORM_CHARSET_CVT_H__

#include <Bex/config.hpp>
#include <Bex/config/windows.h>
#include <boost/locale.hpp>
#include <Bex/utility/singleton.hpp>

//////////////////////////////////////////////////////////////////////////
/// ×Ö·û±àÂë×ª»»
namespace Bex { namespace conv
{
    using boost::locale::conv::between;

    /// windows´úÂëÒ³¶¨Òå
    struct windows_encoding {
        char const *name;
        unsigned codepage;
        unsigned was_tested;
    };

    const windows_encoding all_windows_encodings[] = {
        { "big5", 950, 0 },
        //{ "cp1250", 1250, 0 },
        //{ "cp1251", 1251, 0 },
        //{ "cp1252", 1252, 0 },
        //{ "cp1253", 1253, 0 },
        //{ "cp1254", 1254, 0 },
        //{ "cp1255", 1255, 0 },
        //{ "cp1256", 1256, 0 },
        //{ "cp1257", 1257, 0 },
        //{ "cp874", 874, 0 },
        //{ "cp932", 932, 0 },
        //{ "cp936", 936, 0 },
        { "eucjp", 20932, 0 },
        { "euckr", 51949, 0 },
        { "gb18030", 54936, 0 },
        { "gb2312", 20936, 0 },
        { "gbk", 936, 0 },
        { "iso2022jp", 50220, 0 },
        { "iso2022kr", 50225, 0 },
        { "iso88591", 28591, 0 },
        { "iso885913", 28603, 0 },
        { "iso885915", 28605, 0 },
        { "iso88592", 28592, 0 },
        { "iso88593", 28593, 0 },
        { "iso88594", 28594, 0 },
        { "iso88595", 28595, 0 },
        { "iso88596", 28596, 0 },
        { "iso88597", 28597, 0 },
        { "iso88598", 28598, 0 },
        { "iso88599", 28599, 0 },
        { "koi8r", 20866, 0 },
        { "koi8u", 21866, 0 },
        { "ms936", 936, 0 },
        { "shiftjis", 932, 0 },
        { "sjis", 932, 0 },
        { "usascii", 20127, 0 },
        { "utf8", 65001, 0 },
        { "windows1250", 1250, 0 },
        { "windows1251", 1251, 0 },
        { "windows1252", 1252, 0 },
        { "windows1253", 1253, 0 },
        { "windows1254", 1254, 0 },
        { "windows1255", 1255, 0 },
        { "windows1256", 1256, 0 },
        { "windows1257", 1257, 0 },
        { "windows874", 874, 0 },
        { "windows932", 932, 0 },
        { "windows936", 936, 0 },
    };

    class locale : public singleton<locale>
    {
        std::locale * locale_;

    public:
        ~locale()
        {
            if (locale_)
            {
                delete locale_;
                locale_ = 0;
            }
        }

        operator std::locale const&()
        {
            if (!locale_)
            {
                locale_ = new std::locale();
                *locale_ = boost::locale::generator()(generate_id("en_US."));
            }

            return *locale_;
        }

        void set_locale(std::locale const& loc)
        {
            if (!locale_)
                locale_ = new std::locale(loc);
            else
                *locale_ = loc;
        }

        std::string generate_id(std::string const& lc)
#if defined(BEX_WINDOWS_API)
        {

            int cp = ::GetACP();
            for (int i = 0; i < sizeof(all_windows_encodings) / sizeof(windows_encoding); ++i)
                if (all_windows_encodings[i].codepage == cp)
                {
                    return lc + all_windows_encodings[i].name;
                }
 
            return lc + "utf8";
        }
#else //defined(BEX_WINDOWS_API)
        {
            return lc + "utf8";
        }
#endif //defined(BEX_WINDOWS_API)
    };

    // ansi -> unicode
    inline std::wstring a2w(std::string const& str)
    {
        using namespace boost::locale::conv;
        return to_utf<wchar_t>(str, locale::getInstance(), default_method);
    }

    // unicode -> ansi
    inline std::string w2a(std::wstring const& wstr)
    {
        using namespace boost::locale::conv;
        return from_utf(wstr, locale::getInstance(), default_method);
    }
    template <typename CharType>
    inline std::string w2a(std::basic_string<CharType> const& wstr)
    {
        using namespace boost::locale::conv;
        return from_utf(wstr, locale::getInstance(), default_method);
    }
    
    // ansi -> utf8
    inline std::string a2u8(std::string const& str)
    {
        using namespace boost::locale::conv;
        return to_utf<char>(str, locale::getInstance(), default_method);
    }

    // utf8 -> ansi
    inline std::string u82a(std::string const& str)
    {
        using namespace boost::locale::conv;
        return from_utf(str, locale::getInstance(), default_method);
    }

    // utf8 -> unicode
    inline std::wstring u82w(std::string const& str)
    {
        using namespace boost::locale::conv;
        return utf_to_utf<wchar_t>(str, default_method);
    }

    // unicode -> utf8
    template <typename CharType>
    inline std::string w2u8(std::basic_string<CharType> const& wstr)
    {
        using namespace boost::locale::conv;
        return utf_to_utf<char>(wstr, default_method);
    }

} //namespace conv
} //namespace Bex

#endif //__BEX_PLATFORM_CHARSET_CVT_H__