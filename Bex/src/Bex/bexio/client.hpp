#ifndef __BEX_IO_CLIENT_HPP__
#define __BEX_IO_CLIENT_HPP__

//////////////////////////////////////////////////////////////////////////
/// 有连接协议的客户端
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    template <class Session>
    class client
        : boost::noncopyable
    {
    public:
        typedef client<Session> this_type;

        typedef Session session_type;
        typedef shared_ptr<session_type> session_ptr;
        typedef typename session_type::protocol_type protocol_type;
        typedef typename session_type::callback_type callback_type;

        typedef typename protocol_type::socket socket;
        typedef typename protocol_type::socket_ptr socket_ptr;
        typedef typename protocol_type::resolver resolver;
        typedef typename protocol_type::endpoint endpoint;
        typedef typename protocol_type::allocator allocator;
        typedef typename allocator::template rebind<session_type>::other alloc_session_t;
        typedef typename allocator::template rebind<options>::other alloc_options_t;
        typedef typename allocator::template rebind<callback_type>::other alloc_callback_t;

    public:
        client(io_service & ios, options const& opts)
            : ios_(ios), session_(0)
        {
            opts_ = make_shared_ptr<options, alloc_options_t>(opts);
            callback_ = make_shared_ptr<callback_type, alloc_callback_t>();
        }

        ~client()
        {
            terminate();
            while (session_)
                boost::this_thread::sleep( boost::posix_time::milliseconds(1) );
        }

        // 阻塞式连接
        bool connect(endpoint const& addr)
        {
            if (is_running())
                return false;

            socket_ptr sp = protocol_type::alloc_socket(ios_);
            sp->connect(addr, ec_);
            if (ec_) 
                return false;

            session_ = make_shared_ptr<session_type, alloc_session_t>();
            session_->initialize(sp, opts_, callback_);
            running_.set();
            return true;
        }

        // 逻辑线程loop接口
        void run()
        {
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
            if (session_)
                return session_->send(buffer, size);

            return false;
        }

        // 优雅地关闭连接
        void shutdown()
        {
            if (session_)
                session_->shutdown();
        }

        // 强制地关闭连接
        void terminate()
        {
            if (session_)
                session_->terminate();
        }

        // 错误原因
        error_code get_error_code() const
        {
            return ec_;
        }

    private:
        io_service & ios_;

        // 连接
        session_ptr session_;

        // 连接成功
        sentry<inter_lock> running_;

        // 错误码(连接错误)
        error_code ec_;

        // 选项
        shared_ptr<options> opts_;

        // 回调
        shared_ptr<callback_type> callback_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_CLIENT_HPP__