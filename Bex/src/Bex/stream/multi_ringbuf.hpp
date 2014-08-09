#ifndef __BEX_STREAM_MULTI_RINGBUF_HPP__
#define __BEX_STREAM_MULTI_RINGBUF_HPP__

//////////////////////////////////////////////////////////////////////////
/// 多个ring_buf组成一个动态增长的环形缓冲区

#include "ring_buf.hpp"
#include <list>
#include <numeric>
#include <limits>

namespace Bex { namespace stream
{
    class multi_ringbuf
    {
    public:
        typedef boost::function<ring_buf*(std::size_t capacity)> NewFunc;
        typedef boost::function<void(ring_buf*)> DeleteFunc;
        typedef ring_buf::char_type char_type;

        enum {min_element_capacity = 8, };

    private:
        typedef ring_buf* ring_buf_ptr;
        typedef std::list<ring_buf_ptr> buffer_list;
        typedef buffer_list::iterator pos_type;

        long m_max_buffer_count;
        long m_min_buffer_count;
        std::size_t m_element_capacity;

        long m_buffer_count;
        buffer_list m_buffer_list;
        pos_type m_ppos;
        pos_type m_gpos;
        NewFunc m_newfunc;
        DeleteFunc m_deletefunc;
        boost::recursive_mutex m_pos_mutex;

    private:
        void initialize(std::size_t element_capacity, long min_count, long max_count,
            NewFunc const& newfunc, DeleteFunc const& deletefunc)
        {
            m_buffer_count = 0;
            m_newfunc = newfunc;
            m_deletefunc = deletefunc;
            m_element_capacity = (std::max<std::size_t>)(element_capacity, min_element_capacity);
            m_min_buffer_count = (std::max<long>)(min_count, 1);
            m_max_buffer_count = (std::max<long>)(max_count, m_min_buffer_count);
            for (long i = 0; i < m_min_buffer_count; ++i)
                allocate();
            m_ppos = m_buffer_list.begin();
            m_gpos = m_buffer_list.begin();
        }

    public:
        multi_ringbuf(NewFunc const& newfunc, DeleteFunc const& deletefunc,
            std::size_t element_capacity, 
            long min_count = 1, 
            long max_count = (std::numeric_limits<long>::max)())
        {
            initialize(element_capacity, min_count, max_count, newfunc, deletefunc);
        }

        multi_ringbuf(std::size_t element_capacity, 
            long min_count = 1, 
            long max_count = (std::numeric_limits<long>::max)(),
            NewFunc const& newfunc = &multi_ringbuf::new_ringbuf, 
            DeleteFunc const& deletefunc = &multi_ringbuf::delete_ringbuf )
        {
            initialize(element_capacity, min_count, max_count, newfunc, deletefunc);
        }

        ~multi_ringbuf()
        {
            boost::recursive_mutex::scoped_lock lock(m_pos_mutex);

            for (buffer_list::iterator it = m_buffer_list.begin();
                    it != m_buffer_list.end(); ++it)
                m_deletefunc(*it);

            m_buffer_list.clear();
        }

        inline std::size_t capacity() const
        {
            return (m_element_capacity * m_buffer_count);
        }

        inline std::size_t size()
        {
            std::size_t posdiff = (std::distance)(m_gpos, m_ppos);
            if (!posdiff)
                return gbuf()->size();

            return gbuf()->size() + pbuf()->size() + (posdiff - 1) * m_element_capacity;
        }

        inline std::size_t spare()
        {
            std::size_t posdiff = (std::distance)(m_ppos, m_buffer_list.end());
            BOOST_ASSERT(posdiff > 0);
            return pbuf()->spare() + (posdiff - 1) * m_element_capacity
                + m_element_capacity * (m_max_buffer_count - m_buffer_count);
        }

        inline char_type * pptr()
        {
            psync();
            return pbuf()->pptr();
        }

        inline char_type * gptr()
        {
            gsync();
            return gbuf()->gptr();
        }

        void gbump(std::size_t offset)
        {
            gbuf()->gbump(offset);
            gsync();
        }

        void pbump(std::size_t offset)
        {
            pbuf()->pbump(offset);
            psync();
        }

        inline std::size_t gcount()
        {
            gsync();
            return gbuf()->gcount();
        }

        inline std::size_t pcount()
        {
            psync();
            return pbuf()->pcount();
        }

        std::size_t sputn(char_type const* pData, std::size_t uSize)
        {
            char_type const* ptr = pData;
            std::size_t len = uSize;
            psync();
            while (len)
            {
                std::size_t count = pbuf()->sputn(ptr, len);
                psync();
                if (0 == count)
                    return (uSize - len);

                ptr += count;
                len -= count;
            }

            return uSize;
        }

        std::size_t sgetn(char_type * pData, std::size_t uSize)
        {
            char_type * ptr = pData;
            std::size_t len = uSize;
            gsync();
            while (len)
            {
                std::size_t count = gbuf()->sgetn(ptr, len);
                gsync();
                if (0 == count)
                    return (uSize - len);

                ptr += count;
                len -= count;
            }

            return uSize;
        }

    private:
        bool allocate()
        {
            boost::recursive_mutex::scoped_lock lock(m_pos_mutex);

            if (m_buffer_count >= m_max_buffer_count)
                return false;

            ring_buf_ptr pBuffer = m_newfunc(m_element_capacity);
            BOOST_ASSERT(pBuffer);
            m_buffer_list.push_back(pBuffer);
            ++ m_buffer_count;
            return true;
        }

        void release(pos_type pos)
        {
            boost::recursive_mutex::scoped_lock lock(m_pos_mutex);

            if (m_buffer_count > m_min_buffer_count)
            {
                m_deletefunc(*pos);
                m_buffer_list.erase(pos);
                -- m_buffer_count;
            }
            else
            {
                m_buffer_list.push_back(*pos);
                m_buffer_list.erase(pos);
            }
        }

        inline ring_buf_ptr pbuf() const
        {
            return (*m_ppos);
        }

        inline ring_buf_ptr gbuf() const
        {
            return (*m_gpos);
        }

        void pposbump()
        {
            boost::recursive_mutex::scoped_lock lock(m_pos_mutex);
            ++ m_ppos;
            BOOST_ASSERT(m_gpos != m_ppos);
        }

        void gposbump()
        {
            boost::recursive_mutex::scoped_lock lock(m_pos_mutex);
            BOOST_ASSERT(m_gpos != m_ppos);
            release(m_gpos++);
        }

        void psync()
        {
            if (!pbuf()->full())
                return ;

            std::size_t posdiff = (std::distance)(m_ppos, m_buffer_list.end());
            if (posdiff <= 1)
                if (!allocate())
                    return ;

            pposbump();
        }

        void gsync()
        {
            ring_buf_ptr gpos = gbuf();
            ring_buf_ptr ppos = pbuf();
            if (gpos == ppos || !gpos->empty())
                return ;

            gposbump();
        }

    private:
        static ring_buf * new_ringbuf(std::size_t capacity)
        {
            BOOST_ASSERT(capacity > 1);
            char * pData = new char[capacity];
            ring_buf * pBuf = new ring_buf(pData, capacity);
            BOOST_ASSERT(pBuf);
            return pBuf;
        }

        static void delete_ringbuf(ring_buf * ptr)
        {
            BOOST_ASSERT(ptr);
            delete[] ptr->begin();
            delete ptr;
        }
    };

} //namespace stream

    namespace {
        using stream::multi_ringbuf;
    }

} //namespace Bex

#endif //__BEX_STREAM_MULTI_RINGBUF_HPP__