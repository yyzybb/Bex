#include "TestPCH.h"
#include <Bex/thread.hpp>

BOOST_AUTO_TEST_SUITE(s_threadlock_suite)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_threadlock_case)
{
    XDump("开始测试 threadlock");

    /// inter_lock
    {
        inter_lock lock;
        BOOST_CHECK(!lock.is_locked());
        BOOST_CHECK(!lock.unlock());
        BOOST_CHECK(lock.try_lock());

        BOOST_CHECK(!lock.try_lock());
        BOOST_CHECK(lock.is_locked());
        BOOST_CHECK(lock.unlock());

        BOOST_CHECK(!lock.unlock());
        BOOST_CHECK(lock.try_lock());
        BOOST_CHECK(lock.unlock());
        BOOST_CHECK(!lock.is_locked());

        {
            inter_lock::try_scoped scoped(lock);
            BOOST_CHECK(scoped.is_locked());
            BOOST_CHECK(lock.is_locked());
        }

        BOOST_CHECK(!lock.is_locked());
    }

    /// generic_lock
    {
        // mutex
        {
            boost::mutex mu;
            generic_lock gl(mu);

            {
                lock_scoped sc(gl);
                BOOST_CHECK_EQUAL(mu.try_lock(), false);
            }

            BOOST_CHECK_EQUAL(mu.try_lock(), true);
            gl.unlock();
            BOOST_CHECK_EQUAL(mu.try_lock(), true);
            gl.unlock();
        }

        // assert_lock
        {
            assert_lock mu;
            generic_lock gl(mu);

            gl.lock();

#ifndef _DEBUG
            // assert
            gl.lock();
#endif //_DEBUG
        }
    }

    XDump("结束测试 threadlock");
}

BOOST_AUTO_TEST_SUITE_END()