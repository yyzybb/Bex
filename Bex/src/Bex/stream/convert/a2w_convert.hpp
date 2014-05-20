#ifndef __BEX_STREAM_CONVERT_A2W_CONVERT_HPP__
#define __BEX_STREAM_CONVERT_A2W_CONVERT_HPP__

#include "base_convert.hpp"
#include <Bex/platform/charset_cvt.h>
#include <string>
#include <boost/static_assert.hpp>
//////////////////////////////////////////////////////////////////////////
/// ansi -> unicodeºó´æ´¢
namespace Bex { namespace convert
{
    // string -> wstring
    template <class Archive>
    class a2w_convert_string;

    template <class Archive>
    a2w_convert_string<Archive> a2w_cvt(std::string & str, Archive * = 0);

    template <class Archive>
    class a2w_convert_string
        : public basic_serialize_convert<Archive, a2w_convert_string<Archive> >
    {
        typedef basic_serialize_convert<Archive, a2w_convert_string<Archive> > base_type;
        typedef typename base_type::oper_tag oper_tag;

        friend a2w_convert_string<Archive> a2w_cvt<Archive>(std::string & str, Archive * /*= 0*/);

        std::string & m_ansi;
        std::wstring  m_unicode;
        mutable bool  m_owner;

    private:
        explicit a2w_convert_string(std::string & ansi)
            : m_ansi(ansi), m_owner(true)
        {
        }

        a2w_convert_string(a2w_convert_string const& other)
            : m_ansi(other.m_ansi), m_owner(other.m_owner)
        {
            other.m_owner = false;
        }

    public:
        ~a2w_convert_string()
        {
            if (m_owner)
                deconstruct(oper_tag());
        }

        inline std::string& get(unkown_oper_tag) { return m_ansi; }
        inline std::wstring& get(load_oper_tag) { return m_unicode; }
        inline std::wstring& get(save_oper_tag)
        {
            m_unicode = a2w(m_ansi);
            return m_unicode;
        }

    private:
        inline void deconstruct(unkown_oper_tag) {}
        inline void deconstruct(save_oper_tag) {}
        inline void deconstruct(load_oper_tag)
        {
            m_ansi = w2a(m_unicode);
        }
    };

    template <class Archive>
    a2w_convert_string<Archive> a2w_cvt(std::string & str, Archive * /*= 0*/)
    {
        return a2w_convert_string<Archive>(str);
    }

    // char[N] -> wstring
    template <class Archive, int N>
    class a2w_convert_array;

    template <class Archive, int N>
    a2w_convert_array<Archive, N> a2w_cvt(char(&str)[N], Archive * = 0);

    template <class Archive, int N>
    class a2w_convert_array
        : public basic_serialize_convert<Archive, a2w_convert_array<Archive, N> >
    {
        typedef basic_serialize_convert<Archive, a2w_convert_array<Archive, N> > base_type;
        typedef typename base_type::oper_tag oper_tag;

        BOOST_STATIC_ASSERT(N > 0);
        friend a2w_convert_array<Archive, N> a2w_cvt<Archive, N>(char(&str)[N], Archive * /*= 0*/);
        typedef char(&Array)[N];

        Array & m_ansi;
        std::wstring m_unicode;
        mutable bool m_owner;

    private:
        explicit a2w_convert_array(Array & ansi)
            : m_ansi(ansi), m_owner(true)
        {
        }

        a2w_convert_array(a2w_convert_array const& other)
            : m_ansi(other.m_ansi), m_owner(other.m_owner)
        {
            other.m_owner = false;
        }

    public:
        ~a2w_convert_array()
        {
            if (m_owner)
                deconstruct(oper_tag());
        }

        inline Array& get(unkown_oper_tag) { return m_ansi; }
        inline std::wstring& get(load_oper_tag) { return m_unicode; }
        inline std::wstring& get(save_oper_tag)
        {
            m_unicode = a2w(std::string(m_ansi, N));
            return m_unicode;
        }

    private:
        inline void deconstruct(unkown_oper_tag) {}
        inline void deconstruct(save_oper_tag) {}
        inline void deconstruct(load_oper_tag)
        {
            std::string strAnsi = w2a(m_unicode);
            std::size_t ls = (std::min<std::size_t>)(strAnsi.length(), N - 1);
            memcpy(&m_ansi[0], strAnsi.c_str(), ls);
            m_ansi[ls] = 0;
        }
    };

    template <class Archive, int N>
    a2w_convert_array<Archive, N> a2w_cvt(char(&str)[N], Archive * /*= 0*/)
    {
        return a2w_convert_array<Archive, N>(str);
    }

} //namespace convert

namespace {
    using convert::a2w_cvt;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_CONVERT_A2W_CONVERT_HPP__