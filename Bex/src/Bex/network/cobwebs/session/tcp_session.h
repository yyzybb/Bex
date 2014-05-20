#ifndef __BEX_NETWORK_COBWEBS_TCP_SESSION_H__
#define __BEX_NETWORK_COBWEBS_TCP_SESSION_H__

//////////////////////////////////////////////////////////////////////////
/// tcp连接

#include <Bex/network/cobwebs/session/session_base.hpp>
#include <Bex/network/cobwebs/session/session_factory.hpp>
#include <Bex/network/cobwebs/core/register.hpp>
#include <Bex/stream.hpp>

namespace Bex { namespace cobwebs
{
    class tcp_session
        : public session_base
        , public boost::enable_shared_from_this<tcp_session>
    {
        template <typename Session>
        friend class session_factory;

    public:
        typedef tcp::socket socket_type;
        typedef boost::shared_ptr<socket_type> socket_ptr;
        
    private:
        socket_ptr m_socket;                // tcp socket
        multi_ringbuf m_sendbuf;            // 发送缓冲区
        multi_ringbuf m_recvbuf;            // 接收缓冲区
        boost::mutex m_send_mutex;          // 发送数据锁(send接口一次只能一个线程调用)

        bool       m_notify_connected;      // 是否需要调用on_connected回调
        inter_lock m_receive_relay;         // 接收请求接力
        inter_lock m_send_relay;            // 发送请求接力
        
        inter_lock m_shutdown_lock;         // 优雅地关闭中...
        inter_lock m_receive_closed;        // 接收通道已关闭
        inter_lock m_send_closed;           // 发送通道已关闭
        inter_lock m_both_closed;           // socket已关闭(close()中会设置)

        inter_lock m_set_error_lock;        // 错误信息设置锁(用以保证只会设置一次)
        boost::system::error_code m_error;  // 错误信息

        CRegisterCore m_register_core;      // -- 构造时要最后一个初始化, 析构时第一个清理.

    public:
        tcp_session(socket_ptr pSock, session_initialized const& si);
        ~tcp_session();

        /// 发送数据
        virtual bool send(char const* buf, std::size_t bytes);

    public:
        /// 接收线程推进
        virtual void run();

        /// 优雅地关闭连接(主动)
        virtual void shutdown();

        /// 强制关闭
        virtual void close();

    protected:
        /// 连接成功回调
        virtual void on_connected() = 0;

        /// 接收数据成功回调
        virtual void on_recv(char * buf, std::size_t bytes) = 0;

        /// 断开连接回调
        virtual void on_disconnect(boost::system::error_code const& ec) = 0;

    private:
        /// 初始化
        void initialize();

        /// 发起接收请求
        void post_recv(bool relay);

        /// 发起发送请求
        void post_send(bool relay);

        /// @{ 仅用于调试, 请慎重重写
    protected:
        /// 接收回调
        virtual void recv_handler(boost::system::error_code const& ec, std::size_t bytes);

        /// 发送回调
        virtual void send_handler(boost::system::error_code const& ec, std::size_t bytes);
        /// }@

    private:
        /// 连接成功
        void connect_invoke();

        /// 断开连接
        void set_disconnect_error(boost::system::error_code const& ec);

        /// 优雅地关闭连接(被动)
        void passive_shutdown(socket_base::shutdown_type what);

        /// 尝试关闭连接
        void try_shutdown();
    };

    typedef boost::shared_ptr<tcp_session> tcp_session_ptr;
    typedef boost::weak_ptr<tcp_session> tcp_session_holder;

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_TCP_SESSION_H__