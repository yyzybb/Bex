#ifndef __BEX_THREAD_POOL_HPP__
#define __BEX_THREAD_POOL_HPP__

#include <Bex/config.hpp>
#include <Bex/platform.hpp>
#include <Bex/base.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#if defined(BEX_SUPPORT_CXX11)
# include <atomic>
#endif

//////////////////////////////////////////////////////////////////////////
/// 线程池

namespace Bex
{
    class ThreadPool
        : public boost::noncopyable
    {
    public:
        typedef  boost::function<void()>  Task;

        class thread_pool_impl
        {
        public:
            class TSQueue
            {
            public:
                inline void push(Task const& task)
                {
                    boost::mutex::scoped_lock lock(m_mutex);
                    m_taskList.push_back(task);
                }

                inline void pop(Task & out_task)
                {
                    if ( empty() )
                    {
                        out_task = 0;
                        return ;
                    }

                    boost::mutex::scoped_lock lock(m_mutex);
                    if ( empty() )
                    {
                        out_task = 0;
                        return ;
                    }

                    out_task = m_taskList.front();
                    m_taskList.pop_front();
                }

                inline bool empty() const
                {
                    return m_taskList.empty();
                }

            private:
                std::list<Task>         m_taskList;     ///< 任务队列
                boost::mutex            m_mutex;        ///< 任务队列锁
            };

        public:
            thread_pool_impl(uint32_t threads)
                : m_unfinished(0)
            {
                if (!threads)
                    threads = boost::thread::hardware_concurrency();

                for ( uint32_t ui = 0; ui < threads; ++ui )
                    m_tg.create_thread(boost::bind(&thread_pool_impl::run, this));
            }

            ~thread_pool_impl()
            {
                m_tg.interrupt_all();
                m_tg.join_all();
            }

            /// 添加任务到线程池中
            inline void push_task(Task const& task)
            {
#if !defined(BEX_SUPPORT_CXX11)
                BOOST_INTERLOCKED_INCREMENT(&m_unfinished);
#else
                ++ m_unfinished;
#endif
                m_taskQueue.push(task);
            }

            /// 等待所有任务处理完成
            inline bool join_all(uint64_t milliseconds)
            {
                boost::timer bt;
                while ( unfinished() > 0 )
                {
                    if ((std::numeric_limits<uint64_t>::max)() != milliseconds
                        && bt.elapsed() * CLOCKS_PER_SEC > milliseconds)
                        return false;

                    sys_sleep(1);
                }

                return true;
            }

            /// 当前线程池是否空闲
            inline bool is_free() const
            {
                return (0 == m_unfinished);
            }

            /// 当前未完成的任务数量
            inline uint32_t  unfinished() const
            {
                return m_unfinished;
            }

            /// 线程数量
            uint32_t  thread_count() const
            {
                return (uint32_t)m_tg.size();
            }

        private:
            /// 任务处理函数(线程执行函数)
            void run()
            {
                while ( true )
                {
                    if ( m_taskQueue.empty() )
                    {
                        sys_sleep(1);
                        continue;
                    }

                    Task task;
                    m_taskQueue.pop(task);
                    if ( !task )
                        continue;

                    task();
#if !defined(BEX_SUPPORT_CXX11)
                    BOOST_INTERLOCKED_DECREMENT(&m_unfinished);
#else
                    --m_unfinished;
#endif
                }
            }

        private:
            boost::thread_group     m_tg;           ///< 线程组
            TSQueue                 m_taskQueue;    ///< 线程安全的任务队列
#if !defined(BEX_SUPPORT_CXX11)
            volatile long           m_unfinished;   ///< 未完成任务数量
#else
            std::atomic<long>       m_unfinished;
#endif
        };

        ThreadPool(uint32_t threads = 0)
            : m_Impl( new thread_pool_impl(threads) )
        {
        }

        ~ThreadPool()
        {
            if ( m_Impl ) delete m_Impl, m_Impl = NULL;
        }

        /// 添加任务到线程池中
        inline void push_task(Task const& task)
        {
            m_Impl->push_task(task);
        }

        /// 等待所有任务处理完成
        inline bool join_all(uint64_t milliseconds = (std::numeric_limits<uint64_t>::max)())
        {
            return m_Impl->join_all(milliseconds);
        }

        /// 当前线程池是否空闲
        inline bool is_free() const
        {
            return m_Impl->is_free();
        }

        /// 当前未完成的任务数量
        inline uint32_t  unfinished() const
        {
            return m_Impl->unfinished();
        }

        /// 线程数量
        inline uint32_t  thread_count() const
        {
            return m_Impl->thread_count();
        }

    private:
        thread_pool_impl * m_Impl;
    };
}

#endif //__BEX_THREAD_POOL_H__