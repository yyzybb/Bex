#ifndef __BEX_THREAD_LOCK_INTER_LOCK_HPP__
#define __BEX_THREAD_LOCK_INTER_LOCK_HPP__

#include <boost/detail/interlocked.hpp>

namespace Bex
{
    class inter_lock
    {
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
                while (!lock.try_lock()) ;
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

} //namespace Bex

#endif //__BEX_THREAD_LOCK_INTER_LOCK_HPP__