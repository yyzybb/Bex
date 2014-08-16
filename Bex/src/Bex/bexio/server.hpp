#ifndef __BEX_IO_SERVER_HPP__
#define __BEX_IO_SERVER_HPP__

//////////////////////////////////////////////////////////////////////////
/// 有连接协议的服务端
#include "bexio_fwd.hpp"
#include "session_list_mgr.hpp"
#include "handlers.hpp"
#include "core.hpp"

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
        typedef typename session_type::mstrand_service_type mstrand_service_type;

        typedef typename protocol_type::socket socket;
        typedef typename protocol_type::socket_ptr socket_ptr;
        typedef typename protocol_type::acceptor acceptor;
        typedef typename protocol_type::endpoint endpoint;
        typedef typename protocol_type::allocator allocator;
        typedef protocol_traits<protocol_type> protocol_traits_type;

        typedef boost::function<void(error_code const&, endpoint const&)> OnHandshakeError;

    public:
        explicit basic_server(options const& opts
            , io_service & ios = core<allocator>::getInstance().backend())
            : ios_(ios), acceptor_(ios)
        {
            opts_ = make_shared_ptr<options, allocator>(opts);
            callback_ = make_shared_ptr<callback_type, allocator>();
            live_cond_ = make_shared_ptr<origin_condition_type, allocator>(true);
        }

        ~basic_server()
        {
            live_cond_->reset();
            terminate();
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
            for (std::size_t i = 0; i < listen_count; ++i)
                if (!async_accept())
                    return false;

            // 启动工作线程
            use_service<mstrand_service_type>(ios_).startup(opts_->workthread_count);

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

        // 设置连接消息回调
        template <typename session_type::callback_em CallbackType, typename F>
        void set_callback(F const& f)
        {
            session_type::set_callback(*callback_, f);
        }

        // 设置握手出错回调
        void set_handshake_error_callbcak(OnHandshakeError const& f)
        {
            on_handshake_error_ = f;
        }

    private:
        // 发起接受连接请求
        bool async_accept(bool reply = false)
        {
            if (!reply)
            {
                ++ accept_count_;
            }

            error_code ec;
            socket_ptr sp = protocol_type::alloc_socket(ios_, *opts_, ec);
            if (ec || !sp)
            {
                if (ec && !ec_)
                    ec_ = ec;
                return false;
            }

            acceptor_.async_accept(sp->lowest_layer(), 
                condition_handler(live_cond_, BEX_IO_BIND(&this_type::on_async_accept, this, BEX_IO_PH_ERROR, sp)));
            return true;
        }

        // 接受连接请求回调
        void on_async_accept(error_code const& ec, socket_ptr sp)
        {
            if (ec)
            {
                if ((--accept_count_) == 0 && shutdowning_.is_set())
                    shutdown_sessions();
                else
                    async_accept(true);

                return ;
            }

            async_accept(true);
            async_handshake(sp);
        }

        // 握手
        void async_handshake(socket_ptr const& sp)
        {
            auto handler = BEX_IO_BIND(&this_type::on_async_handshake, this, BEX_IO_PH_ERROR, sp);
            if (opts_->ssl_opts)
            {
                auto timed_handler = timer_handler<allocator>(handler, ios_);
                timed_handler.expires_from_now(boost::posix_time::milliseconds(opts_->ssl_opts->handshake_overtime));
                timed_handler.async_wait(BEX_IO_BIND(&this_type::on_async_handshake, this, make_error_code(errc::handshake_overtime), sp));
                protocol_traits_type::async_handshake(sp, ssl::stream_base::server, timed_handler);
            }
            else
                protocol_traits_type::async_handshake(sp, ssl::stream_base::server, handler);
        }

        // 握手回调
        void on_async_handshake(error_code const& ec, socket_ptr sp)
        {
            if (ec)
            {
                if (on_handshake_error_)
                {
                    use_service<mstrand_service_type>(ios_).actor().post(BEX_IO_BIND(
                        on_handshake_error_, ec, sp->lowest_layer().remote_endpoint()));
                }
                return ;
            }

            /// create session
            session_type * session_p = allocate<session_type, allocator>();
            session_ptr session(session_p
                , if_else_handler(live_cond_, BEX_IO_BIND(&this_type::session_deleter, this, _1), deallocator<session_type, allocator>())
                , allocator());
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

            deallocate<allocator>(sp);
        }

        // 优雅地关闭所有连接
        void shutdown_sessions()
        {
            session_mgr_.for_each(BEX_IO_BIND(&session_type::shutdown, _1));
        }

        // 强制地关闭所有连接
        void terminate_sessions()
        {
            session_mgr_.for_each(BEX_IO_BIND(&session_type::terminate, _1));
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
        std::atomic<long> accept_count_;

        // 优雅地关闭中
        sentry<bool> shutdowning_;

        // 选项
        shared_ptr<options> opts_;

        // 回调
        shared_ptr<callback_type> callback_;

        // 握手出错回调
        OnHandshakeError on_handshake_error_;

        // loop模式的回调队列
        io_service callback_list;

        // 标识自身是否存活的condition
        handler_condition_type live_cond_;
    };
    

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SERVER_HPP__