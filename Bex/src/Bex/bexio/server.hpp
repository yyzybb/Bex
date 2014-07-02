#ifndef __BEX_IO_SERVER_HPP__
#define __BEX_IO_SERVER_HPP__

//////////////////////////////////////////////////////////////////////////
/// 有连接协议的服务端
#include "bexio_fwd.hpp"
#include "session_list_mgr.hpp"

namespace Bex { namespace bexio 
{
    template <class Session, class SessionMgr = session_list_mgr<Session> >
    class basic_server
        : boost::noncopyable
    {
    public:
        typedef basic_server<Session> this_type;

        typedef Session session_type;
        typedef SessionMgr session_mgr_type;

        typedef shared_ptr<session_type> session_ptr;
        typedef typename session_type::id session_id;
        typedef typename session_type::protocol_type protocol_type;
        typedef typename session_type::callback_type callback_type;

        typedef typename protocol_type::socket socket;
        typedef typename protocol_type::socket_ptr socket_ptr;
        typedef typename protocol_type::acceptor acceptor;
        typedef typename protocol_type::endpoint endpoint;
        typedef typename protocol_type::allocator allocator;
        typedef typename allocator::template rebind<session_type>::other alloc_session_t;
        typedef typename allocator::template rebind<options>::other alloc_options_t;
        typedef typename allocator::template rebind<callback_type>::other alloc_callback_t;

    public:
        basic_server(io_service & ios, options const& opts)
            : ios_(ios), acceptor_(ios)
        {
            opts_ = make_shared_ptr<options, alloc_options_t>(opts);
            callback_ = make_shared_ptr<callback_type, alloc_callback_t>();
        }

        ~basic_server()
        {
            terminate();
            while (session_mgr_.size())
                boost::this_thread::sleep( boost::posix_time::milliseconds(1) );
        }

        // 启动
        bool startup(endpoint const& addr, std::size_t listen_count = 1)
        {
            if (is_running())
                return false;

            acceptor_.open(addr.protocol(), ec_);
            if (ec_) return false;

            acceptor_.set_option(socket_base::reuse_address(false), ec_);
            if (ec_) return false;

            acceptor_.bind(addr, ec_);
            if (ec_) return false;

            acceptor_.listen(socket_base::max_connections, ec_);
            if (ec_) return false;

            accept_count_ = listen_count;
            for (std::size_t i = 0; i < listen_count; ++listen_count)
                async_accept();

            running_.set();
            return true;
        }

        // 优雅地关闭
        void shutdown()
        {
            // 设置shutdown标记
            if (!shutdowning_.set())
                return ;

            // 先停止接受连接
            stop_accept();
        }

        // 强制关闭
        void terminate()
        {
            stop_accept();
            terminate_sessions();
        }

        // 是否工作中
        bool is_running() const
        {
            return running_.is_set();
        }

        // 根据id查找连接
        session_ptr find(session_id id) const
        {
            return session_mgr_.find(id);
        }

        // 当前连接数
        std::size_t size() const
        {
            return session_mgr_.size();
        }

        // 提取错误信息
        error_code get_error_code() const
        {
            return ec_;
        }

        // 设置回调
        template <typename session_type::callback_em CallbackType, typename F>
        void set_callback(F const& f)
        {
            session_type::set_callback(*callback_, f);
        }

    private:
        // 发起接受连接请求
        void async_accept(bool reply = false)
        {
            if (!reply)
            {
                BOOST_INTERLOCKED_INCREMENT(&accept_count_);
            }

            socket_ptr sp = protocol_type::alloc_socket(ios_);
            acceptor_.async_accept(sp->lowest_layer(), 
                boost::bind(&this_type::on_async_accept, this, placeholders::error, sp));
        }

        // 接受连接请求回调
        void on_async_accept(error_code const& ec, socket_ptr sp)
        {
            if (ec)
            {
                if (BOOST_INTERLOCKED_DECREMENT(&accept_count_) == 1 && shutdowning_.is_set())
                    shutdown_sessions();

                return ;
            }

            async_accept(true);

            /// create session
            session_type * session_p = allocate<session_type, alloc_session_t>();
            session_ptr session(session_p, boost::bind(&this_type::session_deleter, this, _1), alloc_session_t());
            session_p->initialize(sp, opts_, callback_);
            session_mgr_.insert(session);
        }

        // 停止接受连接
        void stop_accept()
        {
            error_code ec;
            acceptor_.cancel(ec);
            acceptor_.close(ec);
        }

        // 连接删除器
        void session_deleter(session_type * sp)
        {
            session_mgr_.erase(sp);
            if (!session_mgr_.size())
            {
                running_.reset();
                shutdowning_.reset();
            }
        }

        // 优雅地关闭所有连接
        void shutdown_sessions()
        {
            session_mgr_.for_each(boost::bind(&session_type::shutdown, _1));
        }

        // 强制地关闭所有连接
        void terminate_sessions()
        {
            session_mgr_.for_each(boost::bind(&session_type::terminate, _1));
        }

    private:
        io_service & ios_;

        // acceptor
        acceptor acceptor_;

        // 连接管理器
        session_mgr_type session_mgr_;

        // 是否工作中
        sentry<inter_lock> running_;

        // 错误码(只记录第一个错误)
        error_code ec_;

        // 执行中的accept请求数
        volatile long accept_count_;

        // 优雅地关闭中
        sentry<bool> shutdowning_;

        // 选项
        shared_ptr<options> opts_;

        // 回调
        shared_ptr<callback_type> callback_;
    };
    

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SERVER_HPP__