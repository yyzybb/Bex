#ifndef __BEX_THREAD_LOCK_ASSERT_LOCK_HPP__
#define __BEX_THREAD_LOCK_ASSERT_LOCK_HPP__

#include <boost/thread/mutex.hpp>
#include <boost/thread/null_mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace Bex
{
    template <class Mutex>
    class abort_lock
    {
        Mutex mu;

    public:
        void lock()
        {
            BOOST_ASSERT(mu.try_lock());
        }

        void unlock()
        {
            mu.unlock();
        }
    };

#ifndef _DEBUG
    typedef boost::null_mutex assert_lock;
    typedef boost::null_mutex recursive_assert_lock;
#else //_DEBUG
    typedef abort_lock<boost::mutex> assert_lock;
    typedef abort_lock<boost::recursive_mutex> recursive_assert_lock;
#endif //_DEBUG

} //namespace Bex

#endif //__BEX_THREAD_LOCK_ASSERT_LOCK_HPP__