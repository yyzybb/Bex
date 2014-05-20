#ifndef __BEX_STREAM_RING_BUF_HPP__
#define __BEX_STREAM_RING_BUF_HPP__

#include "ring_streambuf.hpp"
#include <boost/assert.hpp>
#include <Bex/thread.hpp>

//////////////////////////////////////////////////////////////////////////
/// 环形缓冲区.

/*
* @ 在只有一个reader和一个writer的情况下是线程安全的.
* @ 读/写时无需拷贝.
*/

#if defined(_DEBUG) && !defined(BEX_NO_DEBUG)
# define BEX_DEBUG_RING_BUF
#endif //_DEBUG

namespace Bex { namespace stream
{
    class ring_buf
        : private ring_streambuf
    {
    public:
        typedef ring_streambuf::char_type char_type;

    private:
        typedef ring_streambuf base_type;

        /// lock debug check
#ifdef BEX_DEBUG_RING_BUF
        mutable abort_lock<boost::recursive_mutex> m_gassert_lock;
        mutable abort_lock<boost::recursive_mutex> m_passert_lock;
        mutable generic_lock m_glock;
        mutable generic_lock m_plock;
#endif //BEX_DEBUG_RING_BUF

    public:
        ring_buf(char_type * pBuffer, std::size_t uSize)
            : base_type(pBuffer, uSize)
#ifdef BEX_DEBUG_RING_BUF
            , m_glock(m_gassert_lock), m_plock(m_passert_lock)
#endif //BEX_DEBUG_RING_BUF
        {
            BOOST_ASSERT(pBuffer);
            BOOST_ASSERT(uSize);
        }

#if defined(_MSC_VER) && _MSC_VER >= 1600
        void swap(ring_buf & rhs)
        {
            base_type::swap(static_cast<base_type&>(rhs));
        }
#endif

        inline std::size_t capacity() const
        {
            return base_type::capacity();
        }

        inline std::size_t size() const
        {
            return base_type::size();
        }

        inline std::size_t spare() const
        {
            return capacity() - size() - 1;
        }

        inline bool full() const
        {
            return (0 == spare());
        }

        inline bool empty() const
        {
            return (0 == size());
        }

        inline char_type * pptr() const
        {
            char_type * p = base_type::pptr();
            return (p == end()) ? begin() : p;
        }

        inline char_type * gptr() const
        {
            char_type * g = base_type::gptr();
            return (g == end()) ? begin() : g;
        }

        void gbump(std::size_t offset)
        {
#ifdef BEX_DEBUG_RING_BUF
            generic_lock::scoped lock(m_glock);
#endif //BEX_DEBUG_RING_BUF

            if (base_type::gptr() == end())
                base_type::setg(begin(), begin(), pptr());
            BOOST_ASSERT(gcount() >= offset);
            base_type::gbump(offset);
            gsync();
        }

        void pbump(std::size_t offset)
        {
#ifdef BEX_DEBUG_RING_BUF
            generic_lock::scoped lock(m_plock);
#endif //BEX_DEBUG_RING_BUF

            if (base_type::pptr() == end())
                base_type::setp(begin(), gptr() - 1);
            BOOST_ASSERT(pcount() >= offset);
            base_type::pbump(offset);
            psync();
        }

        inline std::size_t gcount() const
        {
#ifdef BEX_DEBUG_RING_BUF
            generic_lock::scoped lock(m_glock);
#endif //BEX_DEBUG_RING_BUF

            char_type * p = pptr(), * g = gptr();
            return (p >= g) ? (p - g) : (end() - g);
        }

        inline std::size_t pcount() const
        {
#ifdef BEX_DEBUG_RING_BUF
            generic_lock::scoped lock(m_plock);
#endif //BEX_DEBUG_RING_BUF

            char_type * p = pptr(), * g = gptr();
            return (g > p) ? (g - p - 1) : 
                (end() - p - (begin() == g ? 1 : 0));
        }

        std::size_t sputn(char_type const* pData, std::size_t uSize)
        {
#ifdef BEX_DEBUG_RING_BUF
            generic_lock::scoped lock(m_plock);
#endif //BEX_DEBUG_RING_BUF
            return (std::size_t)base_type::sputn(pData, uSize);
        }

        std::size_t sgetn(char_type * pData, std::size_t uSize)
        {
#ifdef BEX_DEBUG_RING_BUF
            generic_lock::scoped lock(m_glock);
#endif //BEX_DEBUG_RING_BUF
            return (std::size_t)base_type::sgetn(pData, uSize);
        }

        inline char_type * begin() const
        {
            return base_type::begin();
        }

        inline char_type * end() const
        {
            return base_type::end();
        }

    protected:
        inline void gsync()
        {
            char * p = pptr();
            char * g = gptr();
            base_type::setg(g, g, (p > g ? p : end()));
        }

        inline void psync()
        {
            char * p = pptr();
            char * g = gptr();
            base_type::setp(p, (p < g ? (g - 1) : (g == begin() ? (end() - 1) : end())));
        }
    };

} /*namespace stream*/ 

namespace {
    using stream::ring_buf;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_RING_BUF_HPP__