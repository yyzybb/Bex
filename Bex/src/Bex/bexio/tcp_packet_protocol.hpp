#ifndef __BEX_IO_TCP_PACKET_PROTOCOL_HPP__
#define __BEX_IO_TCP_PACKET_PROTOCOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// tcp packeted socket policy
#include "bexio_fwd.hpp"
#include "buffered_socket.hpp"
#include "nonblocking_circularbuffer.hpp"
#include "allocator.hpp"
#include "packet_parser.hpp"

namespace Bex { namespace bexio
{
    template <typename Parser = packet_parser<>,
        typename Buffer = nonblocking_circularbuffer>
    struct tcp_packet_protocol
    {
        typedef Buffer buffer_type;
        typedef Parser parser_type;
        typedef typename parser_type::allocator allocator;
        typedef typename parser_type::packet_head_type packet_head_type;

        typedef ip::tcp::endpoint endpoint;
        typedef ip::tcp::acceptor acceptor;
        typedef ip::tcp::resolver resolver;
        typedef ip::tcp::socket native_socket_type;
        typedef buffered_socket<native_socket_type, buffer_type, allocator> socket;
        typedef boost::shared_ptr<socket> socket_ptr;

        // callback functions
        typedef boost::function<void(error_code const&, packet_head_type*, char const*, std::size_t)> OnReceiveF;

        static socket_ptr alloc_socket(io_service & ios, options & opts, error_code & ec)
        {
            ec.clear();
            return make_shared_ptr<socket, allocator>(ios
                , opts.receive_buffer_size, opts.send_buffer_size);
        }

    protected:
        /// 用于让用户自定义session重写的接收数据函数
        virtual void on_receive(error_code const&, packet_head_type*, char const*, std::size_t) {}

        //////////////////////////////////////////////////////////////////////////
        /// @{ 只有session可以调用这个接口
    public:
        /// 初始化
        template <typename F, typename Id>
        void initialize(shared_ptr<options> const& opts, F const& f, Id const& id)
        {
            opts_ = opts;
            parser_.initialize(opts_->max_packet_size, BEX_IO_BIND(&tcp_packet_protocol::on_parse, this, _1, _2, _3, _4));
            if (f)
                global_receiver_ = BEX_IO_BIND(f, id, _1, _2, _3, _4);
        }

    protected:
        /// 用于传递接收到的数据至解析器
        inline void parse(const_buffer const& buffer)
        {
            parser_.parse((char const*)buffer_cast_helper(buffer), buffer_size_helper(buffer));
        }
        /// @}
        //////////////////////////////////////////////////////////////////////////

    private:
        /// 解析回调
        void on_parse(error_code const& ec, packet_head_type* ph, char const* data, std::size_t size)
        {
            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                on_receive(ec, ph, data, size);
            
            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (global_receiver_)
                    global_receiver_(ec, ph, data, size);
        }

    private:
        // 数据解析器
        parser_type parser_;

    protected:
        shared_ptr<options> opts_;
        OnReceiveF global_receiver_;
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_TCP_PACKET_PROTOCOL_HPP__
