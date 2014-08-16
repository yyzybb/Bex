#ifndef __BEX_IO_SESSION_HPP__
#define __BEX_IO_SESSION_HPP__

//////////////////////////////////////////////////////////////////////////
/// 连接

/*
* @仅用于有连接协议
*
* @functions: 带有_cb和_l后缀的接口必须在逻辑线程执行。
*
* @优雅地关闭连接流程:
*   \主动方: 请求关闭(调用shutdown接口) -> 禁止发送(send全部返回false) -> 记录错误码
*       -> 等待应用层发送缓冲区数据全部复制到底层socket缓冲区 -> 关闭发送通道
*       -> 等待对方关闭另外一个通道 -> ...
*   \被动方: 全部数据接收完毕直到eof -> 记录错误码 -> mark被动优雅地关闭 -> 禁止发送(send全部返回false)
*       -> 等待应用层发送缓冲区数据全部复制到底层socket缓冲区 -> if(mark) 关闭发送通道
*       -> do_shutdown_lowest -> 关闭完成
*   \主动方: 全部数据接收完毕直到eof -> do_shutdown_lowest -> 关闭完成
*
*
* @log 2014-07-10: double pingpong测试时, 数据链断档是wait机制导致的, 不是bug...
*
*/

#include "bexio_fwd.hpp"
#include "multithread_strand_service.hpp"
#include "intrusive_list.hpp"
#include "protocol_traits.hpp"
#include "socket_options.hpp"
#include "handlers.hpp"

namespace Bex { namespace bexio
{
    template <class Session>
    struct hook
        : intrusive_list_hook
    {};

    template <typename Protocol,
        template <typename> class Hook = hook
    >
    class basic_session
        : public Protocol
        , public Hook<basic_session<Protocol, Hook> >
        , public boost::enable_shared_from_this<basic_session<Protocol, Hook> >
        , boost::noncopyable
    {
        typedef basic_session<Protocol, Hook> this_type;

    public:
        typedef Protocol protocol_type;
        typedef typename protocol_type::allocator allocator;
        typedef typename protocol_type::socket_ptr socket_ptr;
        typedef typename protocol_type::endpoint endpoint;
        typedef protocol_traits<protocol_type> protocol_traits_type;
        typedef multithread_strand_service<allocator> mstrand_service_type;

        // 连接id
        class id
            : boost::totally_ordered<id>
        {
            typedef this_type session_type;
            friend session_type;

            explicit id(shared_ptr<session_type> const& spointer)
                : id_(spointer ? spointer->id_ : 0), wpointer_(spointer)
            {}

            friend bool operator<(id const& lhs, id const& rhs)
            {
                return lhs.id_ < rhs.id_;
            }

            shared_ptr<session_type> get()
            {
                return wpointer_.lock();
            }

            long id_;
            weak_ptr<session_type> wpointer_;
        };
        friend class id;

        /// 回调类型
        enum BEX_ENUM_CLASS callback_em
        {
            cb_connect = 0,         ///< 连接成功
            cb_disconnect = 1,      ///< 断开连接
            cb_receive = 2,         ///< 收到数据
        };
        typedef callback_em cbe;

        /// 根据Protocol::F添加id参数, 推导出F
        typedef boost::function<void(id)> OnConnectF;
        typedef boost::function<void(id, error_code const&)> OnDisconnectF;
        typedef typename function_addition<id, typename Protocol::OnReceiveF>::type OnReceiveF;

        typedef boost::tuple<OnConnectF, OnDisconnectF, OnReceiveF> callback_type;

        typedef shared_ptr<options> options_ptr;
        typedef shared_ptr<callback_type> callback_ptr;

    public:
        // @remarks: 为用户自定义的子类书写方便, 将本该构造时传入的参数延迟至initialize中。
        //           在initialize之前session是不安全的, 千万不要使用!
        basic_session()
            : id_(svlid++), service_(0)
        {
        }

        void initialize(socket_ptr socket, options_ptr const& opts, callback_ptr const& callbacks)
        {
            opts_ = opts, callbacks_ = callbacks, socket_ = socket;
            protocol_traits_type::initialize(static_cast<protocol_type&>(*this)
                , opts_, boost::get<(int)cbe::cb_receive>(*callbacks_), get_id());

            mstrand_service()->post(BEX_IO_BIND(&this_type::do_onconnect_cb, this, shared_this()));

            if (opts_->use_keepalive)
            {
                error_code ec;
                set_keepalive(socket_->lowest_layer(), opts_->keepalive_idle, opts_->keepalive_interval, ec);
                if (ec && !opts_->ignore_keepalive_startup_fail)
                {
                    ec_ = ec;
                    terminate();
                    return ;
                }
            }

            post_receive();
        }

        // 发送数据
        bool send(char const* data, std::size_t size)
        {
            if (shutdowning_.is_set())
                return false;

            if (socket_->putable_write() < size)
            {
                /// 发送缓冲区溢出
                if (!on_sendbuffer_overflow())
                    return false;
            }

            socket_->sputn(data, size);
            post_send();
            return true;
        }

        template <typename ConstBufferSequence>
        bool send(ConstBufferSequence const& buffers)
        {
            if (shutdowning_.is_set())
                return false;

            std::size_t size = 0;
            for (typename ConstBufferSequence::iterator it = buffers.begin(); 
                it != buffers.end(); ++it)
                size += buffer_size_helper(*it);

            if (socket_->putable_write() < size)
            {
                /// 发送缓冲区溢出
                if (!on_sendbuffer_overflow())
                    return false;
            }

            socket_->sputn(buffers);
            post_send();
            return true;
        }

        // 优雅地关闭连接
        virtual void shutdown()
        {
            shutdown_handshaking_.set();
            shutdowning_.set();
            on_error(make_error_code(errc::initiative_shutdown));

            auto handler = BEX_IO_BIND(&this_type::on_async_shutdown, this, BEX_IO_PH_ERROR, shared_this());
            if (opts_->ssl_opts)
            {
                auto timeout_handler = timer_handler<allocator>(handler, socket_->get_io_service());
                timeout_handler.expires_from_now(boost::posix_time::milliseconds(opts_->ssl_opts->handshake_overtime));
                timeout_handler.async_wait(BEX_IO_BIND(&this_type::on_async_shutdown, this, make_error_code(errc::handshake_overtime), shared_this()));
                protocol_traits_type::async_shutdown(socket_, timeout_handler);
            }
            else
                protocol_traits_type::async_shutdown(socket_, handler);
        }

        // 强制关闭连接(慎用)
        //   发送RST, 不但会丢失本地上层和底层socket发送缓冲区中未发送的数据, 
        //   也会丢失远端底层socket接收缓冲区中未读取的数据。
        virtual void terminate()
        {
            if (!terminating_.set())
                return ;

            on_error(make_error_code(errc::initiative_terminate));
            error_code ec;
            socket_->lowest_layer().set_option(socket_base::linger(true, 0), ec);
            do_shutdown_lowest();
        }

        // 连接是否已断开
        bool is_disconnected() const
        {
            return disconencted_.is_set();
        }

        // 本地网络地址
        endpoint local_endpoint() const
        {
            return socket_->lowest_layer().local_endpoint();
        }

        // 远端网络地址
        endpoint remote_endpoint() const
        {
            return socket_->lowest_layer().remote_endpoint();
        }

        // 获取配置信息
        shared_ptr<options const> get_options() const
        {
            return opts_;
        }

        // 获取socket
        socket_ptr get_socket()
        {
            return socket_;
        }

        // 获取id
        id get_id()
        {
            return id(shared_this());
        }

        // 设置回调
        template <callback_em CallbackType, typename F>
        static void set_callback(callback_type & cb, F const& f)
        {
            boost::get<CallbackType>(cb) = f;
        }

    protected:
        // 连接回调(mlp_derived || mlp_both 生效)
        virtual void on_connect() {}
        
        // 断开连接回调(mlp_derived || mlp_both 生效)
        virtual void on_disconnect(error_code const& ec) {}

    private:
        // 发起异步发送请求
        void post_send(bool reply = false)
        {
            if (!reply && !sending_.set())
                return ;

            bool sendok = socket_->async_write_some(
                BEX_IO_BIND(&this_type::on_async_send, this, BEX_IO_PH_ERROR, BEX_IO_PH_BYTES_TRANSFERRED, shared_this())
                );

            if (!sendok)    ///< 发送缓冲区已空
            {
                sending_.reset();

                if (shutdowning_.is_set() 
                    && !shutdown_handshaking_.is_set()
                    && !socket_->getable_write())   // 已准备关闭
                {
                    close_send();   // 关闭发送通道
                }
                else if (socket_->getable_write()) // double check (防止发送缓冲区中积存数据未能及时发送)
                    post_send();
            }
        }

        // 发起异步接收请求
        void post_receive(bool reply = false)
        {
            if (!reply && !receiving_.set())
                return;

            bool receiveok = socket_->async_read_some(
                BEX_IO_BIND(&this_type::on_async_receive, this, BEX_IO_PH_ERROR, BEX_IO_PH_BYTES_TRANSFERRED, shared_this())
                );

            if (!receiveok)
            {
                receiving_.reset();

                /// 接收缓冲区已满
                if (on_receivebuffer_overflow())
                    post_receive();
            }
        }
        // 通知逻辑线程发起异步接收请求的接口
        void post_receive_cb(shared_ptr<this_type>)
        {
            post_receive();
        }

    private:
        // 异步发送回调
        void on_async_send(error_code ec, std::size_t bytes, shared_ptr<this_type>)
        {
            //Dump("on_async_send, ec:" << ec.value() << " bytes:" << bytes);

            if (ec)
            {
                close_send();
                on_error(ec);
                return ;
            }

            post_send(true);
        }

        // 异步接收回调
        void on_async_receive(error_code ec, std::size_t bytes, shared_ptr<this_type>)
        {
            //Dump("on_async_receive, ec:" << ec.value() << " bytes:" << bytes);

            if (ec) 
            {
                close_receive();

                // eof 对方调用shutdown, 尝试以优雅地方式关闭连接.
                if (ec.value() == 2)
                {
                    shutdowning_.set();
                    on_error(make_error_code(errc::passive_shutdown));
                }
                else
                    on_error(ec);

                return ;
            }

            // on_receive_run回调要在post_receive前面, 才能确保wait模式下不浪费资源.
            if (notify_receive_.set())
                mstrand_service()->post(BEX_IO_BIND(&this_type::on_receive_cb, this, shared_this()));

            post_receive(true);
        }

        // 关闭握手回调
        void on_async_shutdown(error_code const& ec, shared_ptr<this_type>)
        {
            shutdown_handshaking_.reset();
            if (ec)
                do_shutdown_lowest();
            else
            {
                shutdown_dt_ = make_shared_ptr<deadline_timer, allocator>(socket_->get_io_service());
                shutdown_dt_->expires_from_now(boost::posix_time::milliseconds(opts_->shutdown_timeout));
                shutdown_dt_->async_wait(BEX_IO_BIND(&this_type::on_shutdown_overtime, this, BEX_IO_PH_ERROR, shared_this()));
                if (!sending_.is_set() && !socket_->getable_write())
                    close_send();
            }
        }

        // 关闭连接
        void do_shutdown_lowest()
        {
            if (shutdown_dt_)
            {
                error_code ec;
                shutdown_dt_->cancel(ec);
                shutdown_dt_.reset();
            }

            error_code ec;
            socket_->lowest_layer().cancel(ec);
            socket_->lowest_layer().shutdown(socket_base::shutdown_both, ec);
            socket_->lowest_layer().close(ec);
            notify_ondisconnect();
        }

        // shutdown超时后直接关闭连接
        void on_shutdown_overtime(error_code const& ec, shared_ptr<this_type>)
        {
            if (ec)
                return ;

            shutdown_dt_.reset();
            ec_ = make_error_code(errc::shutdown_overtime);   // 强制修改断开错误原因
            terminate();
        }

    private:
        // 接收数据回调(仅在逻辑线程执行!)
        void on_receive_l()
        {
            boost::array<const_buffer, 2> buffers;
            std::size_t sections = socket_->get_buffers(buffers);
            for (std::size_t i = 0; i < sections; ++i)
            {
                do_onreceive_l(buffers[i]);
                socket_->read_done(buffer_size_helper(buffers[i]));
            }
        }
        void on_receive_cb(shared_ptr<this_type>)
        {
            notify_receive_.reset();
            on_receive_l();
        }

        // 错误处理
        void on_error(error_code ec)
        {
            if (ec && !ec_)  //只需记录第一个错误原因
                ec_ = ec;

            /// shutdown is ok?
            if (shutdowning_.is_set())
            {
                if (receiveclosed_.is_set())
                {
                    if (sendclosed_.is_set())   //发送通道已关闭, 可以关闭socket了.
                        do_shutdown_lowest();
                    else if (!sending_.is_set())    //发送已停止, 可以关闭发送通道了.
                        close_send();
                    else if (!socket_->getable_write())  //发送通道未关闭但发送缓冲区已空, 可以关闭发送通道了.
                        close_send();
                }
            }
            else if (ec && terminating_.set())
            {
                do_shutdown_lowest();
            }
        }

    private:
        /// 关闭发送通道(发送缓冲区发送完毕后再执行)
        void close_send()
        {
            sendclosed_.set();
            error_code ec;
            socket_->lowest_layer().shutdown(socket_base::shutdown_send, ec);
        }

        /// 关闭接收通道(关闭后要处理完接收缓冲区中已接收到的数据才可以关闭)
        void close_receive()
        {
            receiveclosed_.set();
            error_code ec;
            socket_->lowest_layer().shutdown(socket_base::shutdown_receive, ec);
        }

    private:
        /// 通知逻辑线程(在逻辑线程执行, 内部根据通知方案做路由)
        void do_onconnect_l()
        {
            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                on_connect();

            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (callbacks_ && boost::get<(int)cbe::cb_connect>(*callbacks_))
                    boost::get<(int)cbe::cb_connect>(*callbacks_)(get_id());
        }
        void do_onconnect_cb(shared_ptr<this_type>)
        {
            do_onconnect_l();
        }

        template <typename ConstBuffer>
        void do_onreceive_l(ConstBuffer const& arg)
        {
            protocol_type::parse(arg);
        }

        void do_ondisconnect_l()
        {
            disconencted_.set();

            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                on_disconnect(ec_);

            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (callbacks_ && boost::get<(int)cbe::cb_disconnect>(*callbacks_))
                    boost::get<(int)cbe::cb_disconnect>(*callbacks_)(get_id(), ec_);
        }
        void do_ondisconnect_cb(shared_ptr<this_type> )
        {
            do_ondisconnect_l();
        }
        void notify_ondisconnect()
        {
            if (!disconect_notified_.set())
                return ;

            mstrand_service()->post(BEX_IO_BIND(&this_type::do_ondisconnect_cb, this, shared_this()));
        }

        /// 缓冲区溢出处理
        // @return: 是否可以继续操作
        bool on_sendbuffer_overflow()
        {
            if (opts_->sboe_ == sboe::sbo_interrupt)
            {
                on_error(make_error_code(errc::sendbuffer_overflow));
                return false;
            }
            else if (opts_->sboe_ == sboe::sbo_wait)
            {
                return false;
            }
            //else if (opts_->sboe_ == sboe::sbo_extend)
            //{
            //    // @todo: extend send buffer
            //    return true;
            //}
            else  // default
                return false;
        }

        bool on_receivebuffer_overflow()
        {
            if (opts_->rboe_ == rboe::rbo_interrupt)
            {
                on_error(make_error_code(errc::receivebuffer_overflow));
                return false;
            }
            else if (opts_->rboe_ == rboe::rbo_wait)
            {
                mstrand_service()->post(BEX_IO_BIND(&this_type::post_receive_cb, this, shared_this()));
                return false;
            }
            //else if (opts_->rboe_ == rboe::rbo_extend)
            //{
            //    // @todo: extend receive buffer
            //    return true;
            //}
            else // default
                return false;
        }

        mstrand_service_type * mstrand_service()
        {
            if (!service_)
                service_ = &(use_service<mstrand_service_type>(socket_->get_io_service()));

            return service_;
        }

        shared_ptr<this_type> shared_this()
        {
            return boost::enable_shared_from_this<basic_session<Protocol, Hook> >::shared_from_this();
        }

    private:
        /// protocol::socket shared_ptr
        socket_ptr socket_;

        /// multithread_strand service reference.
        mstrand_service_type * service_;

        /// 发送请求是否已投递
        sentry<inter_lock> sending_;
        
        /// 接收请求是否已投递
        sentry<inter_lock> receiving_;

        /// 断线是否已通知
        sentry<bool> disconect_notified_;

        /// 发送通道是否关闭
        sentry<bool> sendclosed_;

        /// 接收通道是否关闭
        sentry<bool> receiveclosed_;

        /// 是否优雅地关闭中
        // 状态标志效果: 1.禁用send 2.发送缓冲区中的数据发送完毕后关闭socket发送通道
        sentry<bool> shutdowning_;

        /// 关闭握手中
        sentry<bool> shutdown_handshaking_;

        /// 优雅地关闭超时计时器
        shared_ptr<deadline_timer> shutdown_dt_;

        /// 是否强制关闭中
        sentry<inter_lock> terminating_;

        /// 错误码
        // 只记录第一个错误原因
        error_code ec_;

        /// 连接是否断开
        sentry<bool> disconencted_;

        /// 选项
        options_ptr opts_;

        /// 回调
        callback_ptr callbacks_;

        /// reactor方式, 通知逻辑层receive消息
        sentry<inter_lock> notify_receive_;

        /// session id
        long id_;
        static std::atomic<long> svlid;
    };

    template <typename Protocol,
        template <typename> class Hook
    >
    std::atomic<long> basic_session<Protocol, Hook>::svlid (1);

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SESSION_HPP__