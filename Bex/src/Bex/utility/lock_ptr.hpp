#ifndef __BEX_UTILITY_LOCK_PTR_HPP__
#define __BEX_UTILITY_LOCK_PTR_HPP__

//////////////////////////////////////////////////////////////////////////
/// 锁定式的指针

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>

namespace Bex
{
    template <typename T>
    class lock_ptr
    {
        typedef boost::function<void()> LockFunc;
        typedef boost::function<void()> UnlockFunc;
        T*         m_ptr;

        struct auto_invoke
        {
            UnlockFunc m_func;
            auto_invoke(UnlockFunc func)
                : m_func(func)
            {
            }

            ~auto_invoke()
            {
                if (m_func)
                    m_func();
            }
        };
        boost::shared_ptr<auto_invoke>  m_ulf;

    public:
        template <class Lock>
        lock_ptr(T* ptr, Lock& lock)
            : m_ptr(ptr), m_ulf(new auto_invoke(boost::bind(&Lock::unlock, &lock)))
        {
            lock.lock();            
        }

        lock_ptr(T* ptr, LockFunc const& lf, UnlockFunc const& ulf)
            : m_ptr(ptr), m_ulf(new auto_invoke(ulf))
        {
            lf();
        }

        T* operator->()
        {
            BOOST_ASSERT(m_ptr != 0);
            return m_ptr;
        }

        T& operator*()
        {
            BOOST_ASSERT(m_ptr != 0);
            return *m_ptr;
        }

        T* get()
        {
            return m_ptr;
        }

        operator bool() const
        {
            return (m_ptr != 0);
        }
    };

} //namespace Bex

#endif //__BEX_UTILITY_LOCK_PTR_HPP__