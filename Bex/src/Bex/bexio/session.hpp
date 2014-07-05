#ifndef __BEX_IO_SESSION_HPP__
#define __BEX_IO_SESSION_HPP__

//////////////////////////////////////////////////////////////////////////
/// 连接
/*
* @仅用于有连接协议
*
* @functions: 带有_cb和_l后缀的接口必须在逻辑线程执行。
*/
#include "bexio_fwd.hpp"
#include "multithread_strand_service.hpp"
#include "intrusive_list.hpp"

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
            : id_(BOOST_INTERLOCKED_INCREMENT(&svlid))
        {
        }

        void initialize(socket_ptr socket, options_ptr const& opts, callback_ptr const& callbacks)
        {
            opts_ = opts, callbacks_ = callbacks, socket_ = socket;

            if (opts_->nlte_ == nlte::nlt_reactor)
                mstrand_service()->post(BEX_IO_BIND(&this_type::notify_onconnect_cb, this, shared_this()));
            else
                notify_connect_.set();

            post_receive();
        }

        // actor处理session的完成回调
        void run()
        {
            if (opts_->nlte_ != nlte::nlt_loop)
                return ;

            if (notify_connect_.is_set())
            {
                notify_onconnect_l();
                notify_connect_.reset();
            }
            
            // 查看接收到的数据
            on_receive_l();

            if (!notify_receiving_.is_set())
                post_receive();

            if (notify_disconnect_.is_set())
            {
                notify_ondisconnect_l();
                notify_disconnect_.reset();
            }
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
            shutdowning_.set();
            on_error(generate_error(bee::initiative_shutdown));
        }

        // 强制地关闭连接
        virtual void terminate()
        {
            if (!terminating_.set())
                return ;

            on_error(generate_error(bee::initiative_terminate));
            error_code ec;
            socket_->lowest_layer().cancel(ec);
            socket_->lowest_layer().shutdown(socket_base::shutdown_both, ec);
            socket_->close(ec);
            generate_notify_ondisconnect();
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

    protected:
        // 发起异步发送请求
        void post_send(bool reply = false)
        {
            if (!reply && !sending_.set())
                return ;

            bool sendok = socket_->async_write_some( //post_strand<protocol_type>(*this,
                BEX_IO_BIND(&this_type::on_async_send, this, BEX_IO_PH_ERROR, BEX_IO_PH_BYTES_TRANSFERRED, shared_this())
                //)
                );

            if (!sendok)    ///< 发送缓冲区已空
            {
                sending_.reset();

                if (shutdowning_.is_set())   // 已准备关闭
                {
                    // 关闭发送通道
                    close_send();
                }
                else if (socket_->getable_write()) // double check (防止发送缓冲区中积存数据未能及时发送)
                    post_send();
            }
        }

        // 发起异步接收请求
        void post_receive(bool reply = false)
        {
            if (!reply && !notify_receiving_.set())
                return;

            bool receiveok = socket_->async_read_some( //post_strand<protocol_type>(*this,
                BEX_IO_BIND(&this_type::on_async_receive, this, BEX_IO_PH_ERROR, BEX_IO_PH_BYTES_TRANSFERRED, shared_this())
                //)
                );

            if (!receiveok)
            {
                notify_receiving_.reset();

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
            if (ec) 
            {
                close_receive();
                on_error(ec);
                return ;
            }

            // on_receive_run回调要在post_receive前面, 才能确保wait模式下不浪费资源.
            if (opts_->nlte_ == nlte::nlt_reactor)
                if (notify_receive_.set())
                    mstrand_service()->post(BEX_IO_BIND(&this_type::on_receive_cb, this, shared_this()));
            post_receive(true);
        }

    private:
        // 接收数据回调(仅在逻辑线程执行!)
        void on_receive_l()
        {
            notify_receive_.reset();

            boost::array<const_buffer, 2> buffers;
            std::size_t sections = socket_->get_buffers(buffers);
            for (std::size_t i = 0; i < sections; ++i)
            {
                notify_onreceive_l(buffers[i]);
                socket_->read_done(buffer_size_helper(buffers[i]));
            }

            // 检测是否需要关闭
            on_error(error_code());
        }
        void on_receive_cb(shared_ptr<this_type>)
        {
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
                if (sendclosed_.is_set() && receiveclosed_.is_set() && !socket_->getable_read())
                {
                    /// 可以关闭了
                    error_code ec;
                    socket_->close(ec);
                    generate_notify_ondisconnect();
                }
            }
            else if (ec && !terminating_.is_set())
            {
                terminate();
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
        void notify_onconnect_l()
        {
            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                on_connect();

            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (callbacks_ && boost::get<(int)cbe::cb_connect>(*callbacks_))
                    boost::get<(int)cbe::cb_connect>(*callbacks_)(get_id());
        }
        void notify_onconnect_cb(shared_ptr<this_type>)
        {
            notify_onconnect_l();
        }

        template <typename ConstBuffer>
        void notify_onreceive_l(ConstBuffer const& arg)
        {
            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                protocol_type::on_receive(arg);
            
            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (callbacks_ && boost::get<(int)cbe::cb_receive>(*callbacks_))
                    protocol_type::parse(boost::get<(int)cbe::cb_receive>(*callbacks_), get_id(), arg);
        }

        void notify_ondisconnect_l()
        {
            disconencted_.set();

            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                on_disconnect(ec_);

            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (callbacks_ && boost::get<(int)cbe::cb_disconnect>(*callbacks_))
                    boost::get<(int)cbe::cb_disconnect>(*callbacks_)(get_id(), ec_);
        }
        void notify_ondisconnect_cb(shared_ptr<this_type> )
        {
            notify_ondisconnect_l();
        }
        void generate_notify_ondisconnect()
        {
            if (opts_->nlte_ == nlte::nlt_loop)
                notify_disconnect_.set();
            else if (opts_->nlte_ == nlte::nlt_reactor)
                mstrand_service()->post(BEX_IO_BIND(&this_type::notify_ondisconnect_cb, this, shared_this()));
        }

        /// 缓冲区溢出处理
        // @return: 是否可以继续操作
        bool on_sendbuffer_overflow()
        {
            if (opts_->sboe_ == sboe::sbo_interrupt)
            {
                on_error(generate_error(bee::sendbuffer_overflow));
                return false;
            }
            else if (opts_->sboe_ == sboe::sbo_wait)
            {
                return false;
            }
            else if (opts_->sboe_ == sboe::sbo_extend)
            {
                // @todo: extend send buffer
                return true;
            }
            else  // default
                return false;
        }

        bool on_receivebuffer_overflow()
        {
            if (opts_->rboe_ == rboe::rbo_interrupt)
            {
                on_error(generate_error(bee::receivebuffer_overflow));
                return false;
            }
            else if (opts_->rboe_ == rboe::rbo_wait)
            {
                if (opts_->nlte_ == nlte::nlt_reactor)
                    mstrand_service()->post(BEX_IO_BIND(&this_type::post_receive_cb, this, shared_this()));
                return false;
            }
            else if (opts_->rboe_ == rboe::rbo_extend)
            {
                // @todo: extend receive buffer
                return true;
            }
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
        sentry<inter_lock> notify_receiving_;

        /// 发送通道是否关闭
        sentry<bool> sendclosed_;

        /// 接收通道是否关闭
        sentry<bool> receiveclosed_;

        /// 是否优雅地关闭中
        // 状态标志效果: 1.禁用send 2.发送缓冲区中的数据发送完毕后关闭socket发送通道
        sentry<bool> shutdowning_;

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

        /// loop方式, 通知connect/disconnect消息
        sentry<bool> notify_connect_;
        sentry<bool> notify_disconnect_;

        /// session id
        long id_;
        static volatile long svlid;
    };

    template <typename Protocol,
        template <typename> class Hook
    >
    volatile long basic_session<Protocol, Hook>::svlid = 1;

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SESSION_HPP__