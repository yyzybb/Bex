#ifndef __BEX_THREAD_POOL_HPP__
#define __BEX_THREAD_POOL_HPP__

#include <Bex/platform.hpp>
#include <Bex/base.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

//////////////////////////////////////////////////////////////////////////
/// 线程池

namespace Bex
{
    class ThreadPool
        : public boost::noncopyable
    {
    public:
        typedef  boost::function<void()>  Task;

        ThreadPool(uint32_t threads = 0);
        ~ThreadPool();

        /// 添加任务到线程池中
        void push_task(Task const& task);

        /// 等待所有任务处理完成
        bool join_all(uint64_t milliseconds = (std::numeric_limits<uint64_t>::max)());

        /// 当前线程池是否空闲
        bool is_free() const;

        /// 当前未完成的任务数量
        uint32_t  unfinished() const;

        /// 线程数量
        uint32_t  thread_count() const;

    private:
        class thread_pool_impl;
        thread_pool_impl * m_Impl;
    };
}

#endif //__BEX_THREAD_POOL_H__