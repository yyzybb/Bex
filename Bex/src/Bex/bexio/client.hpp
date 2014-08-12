#ifndef __BEX_IO_CLIENT_HPP__
#define __BEX_IO_CLIENT_HPP__

//////////////////////////////////////////////////////////////////////////
/// 有连接协议的客户端
#include "bexio_fwd.hpp"
#include "handlers.hpp"
#include "core.hpp"

namespace Bex { namespace bexio
{
    template <class Session>
    class basic_client
        : boost::noncopyable
    {
    public:
        typedef basic_client<Session> this_type;

        typedef Session session_type;
        typedef shared_ptr<session_type> session_ptr;
        typedef typename session_type::protocol_type protocol_type;
        typedef typename session_type::callback_type callback_type;
        typedef typename session_type::mstrand_service_type mstrand_service_type;

        typedef typename protocol_type::socket socket;
        typedef typename protocol_type::socket_ptr socket_ptr;
        typedef typename protocol_type::resolver resolver;
        typedef typename protocol_type::endpoint endpoint;
        typedef typename protocol_type::allocator allocator;
        typedef protocol_traits<protocol_type> protocol_traits_type;

        typedef boost::function<void(error_code const&)> OnAsyncConnect;
        typedef boost::function<void(error_code const&, endpoint const&)> OnHandshakeError;

    public:
        explicit basic_client(options const& opts
            , io_service & ios = core<allocator>::getInstance().backend())
            : ios_(ios), mstrand_service_(use_service<mstrand_service_type>(ios))
        {
            opts_ = make_shared_ptr<options, allocator>(opts);
            callback_ = make_shared_ptr<callback_type, allocator>();
        }

        ~basic_client()
        {
            terminate();
            while (session_)
                boost::this_thread::sleep( boost::posix_time::milliseconds(1) );
        }

        // 阻塞式连接
        bool connect(endpoint const& addr)
        {
            if (is_running() || async_connecting_.is_set())
                return false;

            socket_ptr sp = protocol_type::alloc_socket(ios_, *opts_, ec_);
            if (ec_)
                return false;

            sp->lowest_layer().connect(addr, ec_);
            if (ec_) 
                return false;

            protocol_traits_type::handshake(sp, ssl::stream_base::client, ec_);
            if (ec_) 
                return false;

            session_ = make_shared_ptr<session_type, allocator>();
            session_->initialize(sp, opts_, callback_);

            // 启动工作线程
            mstrand_service_.startup(opts_->workthread_count);
            running_.set();
            return true;
        }

        // 设置异步连接回调
        void set_async_connect_callback(OnAsyncConnect const& callback)
        {
            async_connect_callback_ = callback;
        }

        // 异步连接
        bool async_connect(endpoint const& addr)
        {
            if (is_running() || !async_connecting_.set())
                return false;

            socket_ptr sp = protocol_type::alloc_socket(ios_, *opts_, ec_);
            if (ec_)
                return false;

            sp->lowest_layer().async_connect(addr, 
                BEX_IO_BIND(&this_type::on_async_connect, this, BEX_IO_PH_ERROR
                    , sp, addr));

            // 启动工作线程
            mstrand_service_.startup(opts_->workthread_count);
            return true;
        }

        // 带超时的异步连接
        bool async_connect_timed(endpoint const& addr, boost::posix_time::time_duration time)
        {
            if (is_running() || !async_connecting_.set())
                return false;

            socket_ptr sp = protocol_type::alloc_socket(ios_, *opts_, ec_);
            if (ec_)
                return false;

            /// 连接超时计时器, 异步等待
            auto timed_handler = timer_handler<allocator>(BEX_IO_BIND(&this_type::on_async_connect_timed, this, BEX_IO_PH_ERROR, sp, addr, time), ios_);
            timed_handler.expires_from_now(time);
            timed_handler.async_wait(BEX_IO_BIND(&this_type::on_overtime, this, BEX_IO_PH_ERROR, sp, bee::connect_overtime));
            sp->lowest_layer().async_connect(addr, timed_handler);

            // 启动工作线程
            mstrand_service_.startup(opts_->workthread_count);
            return true;
        }

        // 连接是否OK
        bool is_running() const
        {
            return (session_ && !session_->is_disconnected());
        }

        // 发送数据
        bool send(char const* buffer, std::size_t size)
        {
            if (is_running())
                return session_->send(buffer, size);

            return false;
        }

        // 优雅地关闭连接
        void shutdown()
        {
            if (is_running())
                session_->shutdown();
        }

        // 强制地关闭连接
        void terminate()
        {
            if (is_running())
                session_->terminate();
        }

        // 错误原因
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
        // 异步连接回调
        void on_async_connect(error_code const& ec, socket_ptr sp, endpoint addr)
        {
            if (ec)
            {
                // 错误
                ec_ = ec;
                async_connecting_.reset();
                notify_onconnect();    // 通知连接结果
            }
            else if (sp->lowest_layer().remote_endpoint() == sp->lowest_layer().local_endpoint())
            {
                // 回环假链接, 重试.
                async_connecting_.reset();
                if (!async_connect(addr))
                {
                    if (!ec_) ec_ = make_error_code(bee::reconnect_error);
                    notify_onconnect();
                }
            }
            else
            {
                // 连接成功, 握手
                async_handshake(sp, addr);
            }
        }

        // 异步连接回调(带超时)
        void on_async_connect_timed(error_code const& ec, socket_ptr sp
            , endpoint addr, boost::posix_time::time_duration timed)
        {
            if (ec)
            {
                // 错误
                ec_ = ec;
                async_connecting_.reset();
                notify_onconnect();
            }
            else if (sp->lowest_layer().remote_endpoint() == sp->lowest_layer().local_endpoint())
            {
                // 回环假链接, 重试.
                async_connecting_.reset();
                if (!async_connect_timed(addr, timed))
                {
                    if (!ec_) ec_ = make_error_code(bee::reconnect_error);
                    notify_onconnect();
                }
                return ;
            }
            else
            {
                // 连接成功, 握手
                async_handshake(sp, addr);
            }
        }

        // 握手
        void async_handshake(socket_ptr const& sp, endpoint const& addr)
        {
            async_handshaking_.set();
            auto handler = BEX_IO_BIND(&this_type::on_async_handshake, this, BEX_IO_PH_ERROR, sp, addr);
            if (opts_->ssl_opts)
            {
                auto timed_handler = timer_handler<allocator>(handler, ios_);
                timed_handler.expires_from_now(boost::posix_time::milliseconds(opts_->ssl_opts->handshake_overtime));
                timed_handler.async_wait(BEX_IO_BIND(&this_type::on_async_handshake, this, make_error_code(bee::handshake_overtime), sp, addr));
                protocol_traits_type::async_handshake(sp, ssl::stream_base::client, timed_handler);
            }
            else
                protocol_traits_type::async_handshake(sp, ssl::stream_base::client, handler);
        }

        // 握手回调
        void on_async_handshake(error_code const& ec, socket_ptr sp, endpoint addr)
        {
            async_handshaking_.reset();
            if (ec)
            {
                ec_ = ec;
                async_connecting_.reset();
                if (on_handshake_error_)
                    mstrand_service_.post(BEX_IO_BIND(
                        on_handshake_error_, ec, sp->lowest_layer().remote_endpoint()));
            }
            else
            {
                ec_.clear();
                running_.set();
                async_connecting_.reset();
                session_ = make_shared_ptr<session_type, allocator>();
                session_->initialize(sp, opts_, callback_);
            }

            notify_onconnect();
        }


        // 超时回调
        void on_overtime(error_code const& ec, socket_ptr sp, bee error_enum)
        {
            if (ec)
                return ;

            // 超时了
            error_code lec;
            sp->lowest_layer().cancel(lec);
            sp->lowest_layer().shutdown(socket_base::shutdown_both, lec);
            sp->lowest_layer().close(lec);
            async_connecting_.reset();
            ec_ = make_error_code(error_enum);
            notify_onconnect();
        }

        // 通知连接结果
        void notify_onconnect()
        {
            if (async_connect_callback_)
                mstrand_service_.post(BEX_IO_BIND(async_connect_callback_, ec_));
        }

    private:
        io_service & ios_;

        // 连接
        session_ptr session_;

        // 异步连接中
        sentry<inter_lock> async_connecting_;

        // 握手中
        sentry<inter_lock> async_handshaking_;

        // 异步连接回调
        OnAsyncConnect async_connect_callback_;

        // 异步连接回调通知
        sentry<bool> notify_async_connect_;

        // 连接成功
        sentry<inter_lock> running_;

        // 连接错误码
        error_code ec_;

        // 工作线程服务
        mstrand_service_type & mstrand_service_;

        // 选项
        shared_ptr<options> opts_;

        // 回调
        shared_ptr<callback_type> callback_;

        // 握手出错回调
        OnHandshakeError on_handshake_error_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_CLIENT_HPP__