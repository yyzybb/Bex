#ifndef __BEX_STREAM_CONVERT_INC_CONVERT_HPP__
#define __BEX_STREAM_CONVERT_INC_CONVERT_HPP__

#include "base_convert.hpp"

//////////////////////////////////////////////////////////////////////////
/// 自增转换器

/* Remarks:
*    operator++()后save.
*    load后operator--().
*/

namespace Bex { namespace convert
{
    template <class Archive, typename T>
    class inc_convert;

    template <class Archive, typename T>
    inc_convert<Archive, T> inc_cvt(T & i, Archive const* = 0);

    template <class Archive, typename T>
    class inc_convert
        : public basic_serialize_convert<Archive, inc_convert<Archive, T> >
    {
        typedef basic_serialize_convert<Archive, inc_convert<Archive, T> > base_type;
        typedef typename base_type::oper_tag oper_tag;

        T& m_i;
        T  m_tmp;
        mutable bool m_owner;

        friend inc_convert<Archive, T> inc_cvt<Archive, T>(T & i, Archive const* /*= 0*/);

    private:
        explicit inc_convert(T& i)
            : m_i(i), m_owner(true) {}

        inc_convert(inc_convert const& other)
            : m_i(other.m_i), m_owner(other.m_owner)
        {
            other.m_owner = false;
        }

    public:
        ~inc_convert() { if (m_owner) deconstruct(oper_tag()); }
        inline T& get(unkown_oper_tag) { return m_i; }
        inline T& get(load_oper_tag) { return m_i; }
        inline T& get(save_oper_tag)
        {
            m_tmp = m_i;
            ++ m_tmp;
            return m_tmp;
        }

    private:
        inline void deconstruct(unkown_oper_tag) {}
        inline void deconstruct(save_oper_tag) {}
        inline void deconstruct(load_oper_tag)
        {
            -- m_i;
        }
    };

    template <class Archive, typename T>
    inc_convert<Archive, T> inc_cvt(T & i, Archive const* /*= 0*/)
    {
        return inc_convert<Archive, T>(i);
    }

} //namespace convert

namespace {
    using convert::inc_cvt;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_CONVERT_INC_CONVERT_HPP__