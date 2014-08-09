#ifndef __BEX_STREAM_RING_STREAMBUF__
#define __BEX_STREAM_RING_STREAMBUF__

#include <iosfwd>
#include <streambuf>
#include <boost/noncopyable.hpp>

//////////////////////////////////////////////////////////////////////////
/// 环形streambuf.

/*
* @ 在只有一个reader和一个writer的情况下是线程安全的.
*/

namespace Bex { namespace stream
{
    class ring_streambuf
        : public std::streambuf
        , private boost::noncopyable
    {
        char_type * m_buffer;
        std::size_t m_size;

    public:
        ring_streambuf(char_type * pBuffer, std::size_t uSize)
            : m_buffer(pBuffer), m_size(uSize)
        {
            BOOST_ASSERT(pBuffer != 0);
            reset();
        }

#if defined(_MSC_VER) && _MSC_VER >= 1600
        void swap(ring_streambuf & rhs)
        {
            std::swap(m_buffer, rhs.m_buffer);
            std::swap(m_size, rhs.m_size);
            std::streambuf::swap(rhs);
        }
#endif

        inline std::size_t capacity() const
        {
            return m_size;
        }

        inline std::size_t size() const
        {
            char_type * p = pptr();
            char_type * g = gptr();
            if (end() == g) g = begin();
            if (end() == p) p = begin();
            return (p >= g) ? (p - g) : (p + capacity() - g);
        }

        inline void reset()
        {
            setg(begin(), begin(), begin());
            setp(begin(), end() - 1);
        }

    protected:
        int_type underflow()
        {
            if (gptr() == pptr())
                return traits_type::eof();

            if (gptr() == end())
                setg(begin(), begin(), begin());

            if (gptr() == pptr())
                return traits_type::eof();

            if (gptr() < pptr())
                setg(gptr(), gptr(), pptr());
            else
                setg(gptr(), gptr(), end());

            return traits_type::to_int_type(*gptr());
        }

        int_type overflow(int_type c)
        {
            if (pptr() == gptr() - 1 || (pptr() == end() - 1 && gptr() == begin()))
                return traits_type::eof();

            if (pptr() == end())
                setp(begin(), begin());

            if (pptr() == gptr() - 1 || (pptr() == end() - 1 && gptr() == begin()))
                return traits_type::eof();

            if (pptr() < gptr() - 1)
                setp(pptr(), gptr() - 1);
            else if (gptr() > begin())
                setp(pptr(), end());
            else
                setp(pptr(), end() - 1);

            *pptr() = traits_type::to_char_type(c);
            pbump(1);
            return c;
        }

        inline char_type * begin() const
        {
            return m_buffer;
        }

        inline char_type * end() const
        {
            return m_buffer + m_size;
        }
    };

} /*namespace stream*/ 

namespace {
    using stream::ring_streambuf;
} //namespace

} //namespace Bex


#endif //__BEX_STREAM_RING_STREAMBUF__