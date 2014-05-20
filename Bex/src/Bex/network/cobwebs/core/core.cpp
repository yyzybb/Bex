#include "core.h"
#include <boost/thread.hpp>

namespace Bex { namespace cobwebs
{
    class core::impl
    {
        typedef core::RunFunction RunFunction;
        typedef core::PostHandler PostHandler;
        typedef std::list<PostHandler> PostHandlerList;

        boost::shared_ptr<io_service::work> m_worker;
        io_service                          m_ios;
        inter_lock                          m_lock;
        boost::thread_group                 m_thread_group;
        bool                                m_terminate_threads;
        boost::recursive_mutex              m_post_mutex;
        PostHandlerList                     m_post_list;
        boost_signals2::signal<void()>      m_registry;

    public:
        impl()
            : m_terminate_threads(false)
        {
        }

        ~impl()
        {
#ifdef _WIN32
            if (m_terminate_threads)
                boost::asio::detail::win_thread::set_terminate_threads(true);
#endif 
            shutdown();
        }

        // 数据接收线程推进
        inline void recv_run()
        {
            m_registry();

            boost::recursive_mutex::scoped_lock lock(m_post_mutex);
            for (PostHandlerList::iterator it = m_post_list.begin(); it != m_post_list.end(); ++it)
                (*it)();
            m_post_list.clear();
        }

        // 取得io_service
        inline io_service& get_io_service()
        {
            return m_ios;
        }

        // 启动
        bool startup(unsigned int threads)
        {
            if (!m_lock.try_lock())
                return false;

            m_ios.reset();
            m_worker.reset(new io_service::work(m_ios));

            threads = (threads > 0) ? threads : boost::thread::hardware_concurrency();
            for (unsigned int ui = 0; ui < threads; ++ui)
                m_thread_group.create_thread(boost::bind(&io_service::run, &m_ios));
                        
            if (is_in_dll())
                m_terminate_threads = true;
            return true;
        }

        // 是否工作中
        bool is_running() const
        {
            return m_lock.is_locked();
        }

        // 关闭
        void shutdown()
        {
            if (!m_lock.is_locked()) return ;

            m_worker.reset();
            m_ios.stop();
            if (m_thread_group.size())
            {
                m_thread_group.interrupt_all();
                m_thread_group.join_all();
            }
            m_lock.unlock();
        }

        void post(PostHandler const& handler)
        {
            boost::recursive_mutex::scoped_lock lock(m_post_mutex);
            m_post_list.push_back(handler);
        }

        // 注册run
        signal_scoped register_run(RunFunction const& run_func, int group)
        {
            return make_signal_scoped(m_registry.connect(group, run_func));
        }
    };

    core::core()
        : m_impl(new impl)
    {
        BOOST_ASSERT(m_impl.get());
    }

    bool core::startup( unsigned int threads /*= 0*/ )
    {
        BOOST_ASSERT(m_impl.get());
        return m_impl->startup(threads);
    }

    // 是否工作中
    bool core::is_running() const
    {
        BOOST_ASSERT(m_impl.get());
        return m_impl->is_running();
    }

    void core::shutdown()
    {
        BOOST_ASSERT(m_impl.get());
        m_impl->shutdown();
        get_pool().release_memory();
    }

    io_service& core::get_io_service()
    {
        BOOST_ASSERT(m_impl.get());
        return m_impl->get_io_service();
    }

    void core::run()
    {
        BOOST_ASSERT(m_impl.get());
        m_impl->recv_run();
    }

    Pool& core::get_pool()
    {
        return m_pool;
    }

    void core::post(PostHandler const& handler)
    {
        m_impl->post(handler);
    }

    signal_scoped core::register_run( RunFunction const& run_func, int group /*= 0*/ )
    {
        BOOST_ASSERT(m_impl.get());
        return m_impl->register_run(run_func, group);
    }

} //namespace cobwebs
} //namespace Bex