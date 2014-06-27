#ifndef __BEX_IO_SESSION_HPP__
#define __BEX_IO_SESSION_HPP__

#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    using namespace boost::asio;
    using boost::system::error_code;

    template <typename Protocol, typename SessionMgr>
    class session
        : public SessionMgr::hook
        , public Protocol
    {
    public:
        struct id {};

        /// 根据Protocol::F添加id参数, 推导出F
        typedef boost::function<void(error_code)> OnConnectF;
        typedef boost::function<void(error_code)> OnDisconnectF;
        typedef boost::function<void(char const*, std::size_t)> OnReceiveF;

    public:
        explicit session(socket_ptr socket)
            : socket_(socket)
        {
        }

        // actor处理session的完成回调
        void run();

        // 发送数据
        void send(char const* data, std::size_t size)
        {
            socket_->sputn(data, size);
        }

        template <typename ConstBufferSequence>
        void send(ConstBufferSequence const& buffers)
        {
            socket_->sputn(buffers);
        }

        // 优雅地关闭连接
        void shutdown();

        // 强制地关闭连接
        void terminate();

    private:
        // 发起异步发送请求
        void post_send();

        // 异步发送回调
        void on_async_send(error_code ec, std::size_t bytes);

        // 发起异步接收请求
        void post_receive();

        // 异步接收回调
        void on_async_receive(error_code ec, std::size_t bytes, char const* data);

    private:
        // 接收数据回调
        void on_receive();

    private:
        socket_ptr socket_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SESSION_HPP__