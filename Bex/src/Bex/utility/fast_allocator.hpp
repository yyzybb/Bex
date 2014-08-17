#ifndef __BEX_FAST_ALLOCATOR_HPP__
#define __BEX_FAST_ALLOCATOR_HPP__

#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool_alloc.hpp>

namespace Bex
{
    // 线程安全的allocator
    template <typename T>
    using fast_pool_allocator = ::boost::fast_pool_allocator<T>;

    // 非线程安全的allocator, 慎用,慎用!!!
    template <typename T>
    using nonblocking_fast_pool_allocator = ::boost::fast_pool_allocator<T, 
        boost::default_user_allocator_new_delete,
        boost::details::pool::null_mutex>;
} //namespace Bex

#endif //__BEX_FAST_ALLOCATOR_HPP__