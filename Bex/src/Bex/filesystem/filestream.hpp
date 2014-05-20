#ifndef __BEX_FILESYSTEM_FILESTREAM_HPP__
#define __BEX_FILESYSTEM_FILESTREAM_HPP__

#include <fstream>
#include <Bex/platform.hpp>

//////////////////////////////////////////////////////////////////////////
/// 标准库fstream扩展

/* 
* 解决了使用包含ANSI编码的中文路径的文件无法打开的BUG.
*/

namespace Bex
{
template <class Elem,
    class Traits>
    class basic_ifstream
        : ::std::basic_ifstream<Elem, Traits>
    {
        typedef ::std::basic_ifstream<Elem, Traits> base_type;

    public:
        basic_ifstream() {}

        explicit basic_ifstream(const char *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
               : base_type(a2w(_Filename).c_str(), _Mode, _Prot)
        {
        }

        explicit basic_ifstream(const std::string& _Str,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
                : base_type(a2w(_Str).c_str(), _Mode, _Prot)
        {
        }

        explicit basic_ifstream(const wchar_t *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
                : base_type(_Filename, _Mode, _Prot)
        {
        }

        explicit basic_ifstream(const std::wstring& _Str,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
                : base_type(_Str.c_str(), _Mode, _Prot)
        {
        }

#ifdef _NATIVE_WCHAR_T_DEFINED
        explicit basic_ifstream(const unsigned short *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
                : base_type(_Filename, _Mode, _Prot)
        {
        }
#endif /* _NATIVE_WCHAR_T_DEFINED */

        explicit basic_ifstream(_Filet *_File)
            : base_type(_File)
        {
        }

#ifdef _HAS_CPP0X
        basic_ifstream(basic_ifstream&& _Right)
            : base_type(_Right)
        {
        }
#endif //_HAS_CPP0X
    };


template <class Elem,
    class Traits>
    class basic_ofstream
        : ::std::basic_ofstream<Elem, Traits>
    {
        typedef ::std::basic_ofstream<Elem, Traits> base_type;

    public:
        basic_ofstream() {}

        explicit basic_ofstream(const char *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(a2w(_Filename).c_str(), _Mode, _Prot)
        {
        }

        explicit basic_ofstream(const std::string& _Str,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(a2w(_Str).c_str(), _Mode, _Prot)
        {
        }

        explicit basic_ofstream(const wchar_t *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(_Filename, _Mode, _Prot)
        {
        }

        explicit basic_ofstream(const std::wstring& _Str,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(_Str.c_str(), _Mode, _Prot)
        {
        }

#ifdef _NATIVE_WCHAR_T_DEFINED
        explicit basic_ofstream(const unsigned short *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(_Filename, _Mode, _Prot)
        {
        }
#endif /* _NATIVE_WCHAR_T_DEFINED */

        explicit basic_ofstream(_Filet *_File)
            : base_type(_File)
        {
        }

#ifdef _HAS_CPP0X
        basic_ofstream(basic_ofstream&& _Right)
            : base_type(_Right)
        {
        }
#endif //_HAS_CPP0X
    };


template <class Elem,
    class Traits>
    class basic_fstream
        : ::std::basic_fstream<Elem, Traits>
    {
        typedef ::std::basic_fstream<Elem, Traits> base_type;

    public:
        basic_fstream() {}

        explicit basic_fstream(const char *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(a2w(_Filename).c_str(), _Mode, _Prot)
        {
        }

        explicit basic_fstream(const std::string& _Str,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(a2w(_Str).c_str(), _Mode, _Prot)
        {
        }

        explicit basic_fstream(const wchar_t *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(_Filename, _Mode, _Prot)
        {
        }

        explicit basic_fstream(const std::wstring& _Str,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
            : base_type(_Str.c_str(), _Mode, _Prot)
        {
        }

#ifdef _NATIVE_WCHAR_T_DEFINED
        explicit basic_fstream(const unsigned short *_Filename,
            ios_base::openmode _Mode = ios_base::in,
            int _Prot = (int)ios_base::_Openprot)
                : base_type(_Filename, _Mode, _Prot)
        {
        }
#endif /* _NATIVE_WCHAR_T_DEFINED */

        explicit basic_fstream(_Filet *_File)
            : base_type(_File)
        {
        }

#ifdef _HAS_CPP0X
        basic_fstream(basic_fstream&& _Right)
            : base_type(_Right)
        {
        }
#endif //_HAS_CPP0X
    };

    typedef basic_ifstream<char, char_traits<char> > ifstream;
    typedef basic_ofstream<char, char_traits<char> > ofstream;
    typedef basic_fstream<char, char_traits<char> > fstream;

    typedef basic_ifstream<wchar_t, char_traits<wchar_t> > wifstream;
    typedef basic_ofstream<wchar_t, char_traits<wchar_t> > wofstream;
    typedef basic_fstream<wchar_t, char_traits<wchar_t> > wfstream;

} //namespace Bex

#endif //__BEX_FILESYSTEM_FILESTREAM_HPP__