#ifndef __BEX_STREAM_STATIC_STREAMBUF__
#define __BEX_STREAM_STATIC_STREAMBUF__

#include <iosfwd>
#include <streambuf>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <Bex/config.hpp>

//////////////////////////////////////////////////////////////////////////
/// 固定长度的streambuf

namespace Bex { namespace stream
{
    class static_streambuf
        : public ::std::streambuf
        , private boost::noncopyable
    {
        char_type * m_buffer;
        ::std::size_t m_size;

    public:
        static_streambuf(char_type * pBuffer, ::std::size_t uSize, bool bEmpty = true)
            : m_buffer(pBuffer), m_size(uSize)
        {
            BOOST_ASSERT(pBuffer != 0);
            reset(bEmpty);
        }

#if defined(_MSC_VER) && _MSC_VER >= 1600
        void swap(static_streambuf & rhs)
        {
            ::std::swap(m_buffer, rhs.m_buffer);
            ::std::swap(m_size, rhs.m_size);
            ::std::streambuf::swap(rhs);
        }
#endif

        ::std::size_t capacity() const
        {
            return m_size;
        }

        ::std::size_t size() const
        {
            return (pptr() - gptr());
        }

        void reset(bool bEmpty = true)
        {
            if (bEmpty)
            {
                setpg(begin(), begin());
            }
            else
            {
                setpg(end(), begin());
            }
        }

        inline char_type * gptr() const
        {
            return std::streambuf::gptr();
        }

        inline char_type * pptr() const
        {
            return std::streambuf::pptr();
        }

        inline char_type * begin() const
        {
            return (m_buffer);
        }

        inline char_type * end() const
        {
            return (m_buffer + m_size);
        }

    protected:
        int_type underflow()
        {
            if (gptr() < pptr())
            {
                setg(m_buffer, gptr(), pptr());
                return traits_type::to_int_type(*gptr());
            }
            else
            {
                return traits_type::eof();
            }
        }

        int_type overflow(int_type c)
        {
            return traits_type::eof();
        }

        inline char_type * offset(off_type off) const
        {
            return (m_buffer + off);
        }

        inline void setpg(char_type * pp, char_type * gp)
        {
            setp(pp, end());
            setg(gp, gp, pp);
        }

        virtual pos_type seekpos(off_type off
            , ::std::ios_base::openmode which = ::std::ios_base::in | ::std::ios_base::out)
        {
            return seekoff(off, ::std::ios_base::beg, which);
        }

        virtual pos_type seekoff(off_type off
            , ::std::ios_base::seekdir way
            , ::std::ios_base::openmode which = ::std::ios_base::in | ::std::ios_base::out)
        {
            bool b_in = (which & ::std::ios_base::in) != 0;
            bool b_out = (which & ::std::ios_base::out) != 0;
            if (b_in && b_out)
            {
                if (way == ::std::ios_base::cur)
                    return (::std::streampos)(::std::_BADOFF);

                return seekpg(off, way);
            }
            else if (b_in)
                return seekg(off, way);
            else if (b_out)
                return seekp(off, way);
            
            return (::std::streampos)(::std::_BADOFF);
        }

        inline pos_type seekpg(off_type off
            , ::std::ios_base::seekdir way)
        {
            if (way == ::std::ios_base::end)
                off += capacity();

            if (off >= 0 && off <= (off_type)capacity())
            {
                setpg(offset(off), offset(off));
                return (::std::streampos)(off);
            }

            return (::std::streampos)(::std::_BADOFF);
        }

        inline pos_type seekp(off_type off
            , ::std::ios_base::seekdir way)
        {
            if (way == ::std::ios_base::cur)
                off += pptr() - begin();
            else if (way == ::std::ios_base::end)
                off += capacity();

            if (off >= 0 && off <= (off_type) capacity() && off >= gptr() - begin())
            {
                setpg(offset(off), gptr());
                return (::std::streampos)(off);
            }

            return (::std::streampos)(::std::_BADOFF);
        }

        inline pos_type seekg(off_type off
            , ::std::ios_base::seekdir way)
        {
            if (way == ::std::ios_base::cur)
                off += gptr() - begin();
            else if (way == ::std::ios_base::end)
                off += capacity();

            if (off >= 0 && off <= (off_type) capacity() && off <= pptr() - begin())
            {
                setpg(pptr(), offset(off));
                return (::std::streampos)(off);
            }

            return (::std::streampos)(::std::_BADOFF);
        }
    };

} /*namespace stream*/ 

namespace {
    using stream::static_streambuf;
} //namespace

} /*namespace Bex*/

#endif //__BEX_STREAM_STATIC_STREAMBUF__
