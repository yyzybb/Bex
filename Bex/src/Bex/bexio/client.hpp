#ifndef __BEX_IO_CLIENT_HPP__
#define __BEX_IO_CLIENT_HPP__

//////////////////////////////////////////////////////////////////////////
/// 有连接协议的客户端
#include "bexio_fwd.hpp"

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

        typedef boost::function<void(error_code const&)> OnAsyncConnect;

    public:
        basic_client(io_service & ios, options const& opts)
            : ios_(ios), session_(0), mstrand_service_(0)
        {
            opts_ = make_shared_ptr<options, allocator>(opts);
            callback_ = make_shared_ptr<callback_type, allocator>();

            if (opts_->nlte_ == nlte::nlt_reactor)
                mstrand_service_ = &use_service<mstrand_service_type>(ios);
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

            socket_ptr sp = protocol_type::alloc_socket(ios_, opts_->send_buffer_size, opts_->send_buffer_size);
            sp->lowest_layer().connect(addr, ec_);
            if (ec_) 
                return false;

            session_ = make_shared_ptr<session_type, allocator>();
            session_->initialize(sp, opts_, callback_);

            // 启动工作线程
            use_service<mstrand_service_type>(ios_).startup(opts_->workthread_count);
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

            socket_ptr sp = protocol_type::alloc_socket(ios_, opts_->send_buffer_size, opts_->send_buffer_size);
            sp->lowest_layer().async_connect(addr, 
                BEX_IO_BIND(&this_type::on_async_connect, this, BEX_IO_PH_ERROR
                    , sp, addr, shared_ptr<sentry<inter_lock> >(), boost::posix_time::seconds(0)));

            // 启动工作线程
            use_service<mstrand_service_type>(ios_).startup(opts_->workthread_count);
            return true;
        }

        // 带超时的异步连接
        bool async_connect_timed(endpoint const& addr, boost::posix_time::time_duration timed)
        {
            if (is_running() || !async_connecting_.set())
                return false;

            socket_ptr sp = protocol_type::alloc_socket(ios_, opts_->send_buffer_size, opts_->send_buffer_size);

            /// 超时标记
            shared_ptr<sentry<inter_lock> > overtime_token = make_shared_ptr<sentry<inter_lock>, allocator>();

            /// 连接超时计时器, 异步等待
            shared_ptr<deadline_timer> timer_ = make_shared_ptr<deadline_timer, allocator>(ios_);
            timer_->expires_from_now(timed);
            timer_->async_wait(BEX_IO_BIND(&this_type::on_overtime, this, BEX_IO_PH_ERROR, sp, overtime_token, timer_));

            sp->lowest_layer().async_connect(addr, 
                BEX_IO_BIND(&this_type::on_async_connect, this, BEX_IO_PH_ERROR, sp, addr, overtime_token, timed));

            // 启动工作线程
            use_service<mstrand_service_type>(ios_).startup(opts_->workthread_count);
            return true;
        }

        // 逻辑线程loop接口
        void run()
        {
            if (notify_async_connect_.reset() && async_connect_callback_)
                async_connect_callback_(ec_);

            if (session_)
                session_->run();

            if (session_ && session_->is_disconnected())
                session_.reset();
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

    private:
        // 异步连接回调
        void on_async_connect(error_code const& ec, socket_ptr sp
            , endpoint addr, shared_ptr<sentry<inter_lock> > overtime_token
            , boost::posix_time::time_duration timed)
        {
            if (overtime_token && !overtime_token->set())
            {
                // 超时了
                ec_ = generate_error(bee::connect_overtime);
                async_connecting_.reset();
            }
            else if (ec)
            {
                // 错误
                ec_ = ec;
                async_connecting_.reset();
            }
            else if (sp->lowest_layer().remote_endpoint() == sp->lowest_layer().local_endpoint())
            {
                // 回环假链接, 重试.
                async_connecting_.reset();
                if (overtime_token)
                    async_connect_timed(addr, timed);
                else
                    async_connect(addr);
                return ;
            }
            else
            {
                // 连接成功
                ec_.clear();
                running_.set();
                async_connecting_.reset();
                session_ = make_shared_ptr<session_type, allocator>();
                session_->initialize(sp, opts_, callback_);
            }

            if (async_connect_callback_)
            {
                if (opts_->nlte_ == nlte::nlt_reactor)
                    mstrand_service_->post(BEX_IO_BIND(async_connect_callback_, ec_));
                else
                    notify_async_connect_.set();
            }
        }

        // 超时回调
        void on_overtime(error_code const& ec, socket_ptr sp, shared_ptr<sentry<inter_lock> > overtime_token, shared_ptr<deadline_timer>)
        {
            // @todo: 错误码处理

            if (overtime_token->set())
            {
                // 连接超时了
                error_code lec;
                sp->lowest_layer().cancel(lec);
                sp->lowest_layer().close(lec);
            }
        }

    private:
        io_service & ios_;

        // 连接
        session_ptr session_;

        // 异步连接中
        sentry<inter_lock> async_connecting_;

        // 异步连接回调
        OnAsyncConnect async_connect_callback_;

        // 异步连接回调通知
        sentry<bool> notify_async_connect_;

        // 连接成功
        sentry<inter_lock> running_;

        // 连接错误码
        error_code ec_;

        // 工作线程服务
        mstrand_service_type * mstrand_service_;

        // 选项
        shared_ptr<options> opts_;

        // 回调
        shared_ptr<callback_type> callback_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_CLIENT_HPP__