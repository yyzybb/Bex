#ifndef __BEX_IO_STREAM_SESSION_HPP__
#define __BEX_IO_STREAM_SESSION_HPP__

#include "intrusive_list.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace Bex { namespace bexio
{
    using namespace boost::asio;
    using boost::system::error_code;

    template <typename ProtocolTraits>
    class stream_session
        : public intrusive_list<stream_session<ProtocolTraits> >::hook
    {
    public:
        typedef typename ProtocolTraits::implementataion_type::socket_type socket_t;
        typedef typename ProtocolTraits::implementataion_type::context_type context_t;

        typedef boost::shared_ptr<socket_t> socket_ptr;

    public:
        explicit stream_session(socket_ptr socket);

        // actor处理session的完成回调
        void run();

        // 发送数据
        template <typename ConstBufferSequence>
        void send(ConstBufferSequence const& const_buffer_sequence);

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
        void on_receive() BEX_FINAL;

    protected:
        // 连接回调
        virtual void on_connect(error_code ec);

        // 接收数据回调
        virtual void on_receive(const_buffer data);
        
        // 断开连接回调
        virtual void on_disconnect(error_code ec);

    private:
        socket_ptr socket_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_STREAM_SESSION_HPP__