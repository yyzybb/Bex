#ifndef __BEX_THREAD_LOCK_INTER_LOCK_HPP__
#define __BEX_THREAD_LOCK_INTER_LOCK_HPP__

#include <Bex/config.hpp>

#if !defined(BEX_SUPPORT_CXX11)
# include <boost/detail/interlocked.hpp>
#else //!defined(BEX_SUPPORT_CXX11)
# include <atomic>
#endif //!defined(BEX_SUPPORT_CXX11)

//////////////////////////////////////////////////////////////////////////
/// 基于原子操作的自旋锁

namespace Bex
{
    class inter_lock
    {
#if !defined(BEX_SUPPORT_CXX11)
        mutable volatile long value_;

    public:
        inter_lock() : value_(0) 
        {
        }

        bool try_lock()
        {
            return (BOOST_INTERLOCKED_COMPARE_EXCHANGE(&value_, 1, 0) == 0);
        }

        bool unlock()
        {
            return (BOOST_INTERLOCKED_COMPARE_EXCHANGE(&value_, 0, 1) == 1);
        }

        bool is_locked() const
        {
            return (BOOST_INTERLOCKED_COMPARE_EXCHANGE(&value_, 1, 1) == 1);
        }
#else //!defined(BEX_SUPPORT_CXX11)
        std::atomic<int> value_;

    public:
        inter_lock() : value_(0) 
        {
        }

        bool try_lock()
        {
            int except = 0;
            return std::atomic_compare_exchange_weak(&value_, &except, 1);
        }

        bool unlock()
        {
            int except = 1;
            return std::atomic_compare_exchange_weak(&value_, &except, 0);
        }

        bool is_locked() const
        {
            return (value_ == 1);
        }
#endif //!defined(BEX_SUPPORT_CXX11)

        void lock()
        {
            while (!try_lock()) ;
        }

    public:
        class try_scoped
        {
            inter_lock & lock_;
            bool locked_;

        public:
            explicit try_scoped(inter_lock& lock)
                : lock_(lock), locked_(lock.try_lock())
            {
            }

            inline bool is_locked() const
            {
                return locked_;
            }

            inline void detach()
            {
                locked_ = false;
            }

            ~try_scoped()
            {
                if (locked_)
                    lock_.unlock();
            }
        };

        class scoped_lock
        {
            inter_lock & lock_;
            bool locked_;

        public:
            explicit scoped_lock(inter_lock& lock)
                : lock_(lock), locked_(true)
            {
                lock_.lock();
            }

            inline bool is_locked() const
            {
                return locked_;
            }

            inline void detach()
            {
                locked_ = false;
            }

            ~scoped_lock()
            {
                if (locked_)
                    lock_.unlock();
            }
        };
    };

    typedef inter_lock spin_lock;

} //namespace Bex

#endif //__BEX_THREAD_LOCK_INTER_LOCK_HPP__