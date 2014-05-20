#ifndef __BEX_THREAD_LOCK_GENERIC_LOCK_HPP__
#define __BEX_THREAD_LOCK_GENERIC_LOCK_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace Bex
{
    class lock_scoped
        : public boost::noncopyable
    {
        friend class generic_lock;

        class base
        {
            friend class lock_scoped;
        protected:
            base() {}
        public:
            virtual ~base() {}
        };

        template <class Lock>
        class impl : public base
        {
            Lock & lock_;

        public:
            explicit impl(Lock& lock)
                : lock_(lock)
            {
                lock_.lock();
            }

            ~impl()
            {
                lock_.unlock();
            }
        };

        base * m_pImpl;

    public:
        template <class Lock>
        explicit lock_scoped(Lock& lock)
        {
            m_pImpl = new impl<Lock>(lock);
        }

        ~lock_scoped()
        {
            if (m_pImpl) delete m_pImpl;
        }

    private:
        lock_scoped()
            : m_pImpl(0)
        {
        }
    };

    class generic_lock
    {
        boost::function<void()> m_lock;
        boost::function<void()> m_unlock;

    public:
        typedef lock_scoped scoped;

        template <class Lock>
        explicit generic_lock(Lock& lock)
        {
            m_lock = boost::bind(&generic_lock::_lock<Lock>, boost::ref(lock));
            m_unlock = boost::bind(&generic_lock::_unlock<Lock>, boost::ref(lock));
        }

        void lock()
        {
            m_lock();
        }

        void unlock()
        {
            m_unlock();
        }

    private:
        template <class Lock>
        static void _lock(Lock& lock)
        {
            lock.lock();
        }

        template <class Lock>
        static void _unlock(Lock& lock)
        {
            lock.unlock();
        }
    };

} //namespace Bex

#endif //__BEX_THREAD_LOCK_GENERIC_LOCK_HPP__