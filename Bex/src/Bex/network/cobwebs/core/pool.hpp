#ifndef __BEX_NETWORK_COBWEBS_CORE_POOL_HPP__
#define __BEX_NETWORK_COBWEBS_CORE_POOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// 缓冲区内存池

#include <Bex/stream/ring_buf.hpp>
#include <Bex/utility/singleton.hpp>
#include <boost/pool/pool.hpp>

namespace Bex { namespace cobwebs
{
    class Pool
        : private boost::pool<>
    {
        typedef boost::pool<> pool_base;
        boost::mutex m_mutex;

    public:
        Pool()
            : pool_base(1024, 1024)
        {
        }

        void * ordered_malloc(std::size_t bytes)
        {
            boost::mutex::scoped_lock lock(m_mutex);
            return pool_base::ordered_malloc(ceil(bytes));
        }

        void ordered_free(void *const chunk, std::size_t bytes)
        {
            boost::mutex::scoped_lock lock(m_mutex);
            return pool_base::ordered_free(chunk, ceil(bytes));
        }

        ring_buf * alloc_ringbuf(std::size_t bytes)
        {
            char * pData = (char*)ordered_malloc(bytes);
            BOOST_ASSERT(pData);
            return (new ring_buf(pData, bytes));
        }

        void free_ringbuf(ring_buf * ptr)
        {
            if (!ptr) return ;
            char * chunk = ptr->begin();
            if (chunk) ordered_free(chunk, ptr->capacity());
            delete ptr;
        }

        /// 释放所有未使用的内存块
        void release_memory()
        {
            pool_base::release_memory();
        }

    private:
        std::size_t ceil(std::size_t bytes)
        {
            return (bytes / alloc_size() + 
                (bytes % alloc_size() ? 1 : 0));
        }
    };


} //namespace Bex
} //namespace cobwebs

#endif //__BEX_NETWORK_COBWEBS_CORE_POOL_HPP__