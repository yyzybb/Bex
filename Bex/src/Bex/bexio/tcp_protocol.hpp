#ifndef __BEX_IO_TCP_PROTOCOL_HPP__
#define __BEX_IO_TCP_PROTOCOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// tcp socket policy
#include "bexio_fwd.hpp"
#include "buffered_socket.hpp"

namespace Bex { namespace bexio
{
    template <typename Buffer, typename Allocator>
    struct tcp_protocol
    {
        typedef buffered_socket<ip::tcp::socket, Buffer, Allocator> socket;
        typedef boost::shared_ptr<socket> socket_ptr;

        // callback functions
        typedef boost::function<void(socket_ptr)> HandleSocketF;
        typedef boost::function<void(error_code)> OnConnectF;
        typedef boost::function<void(error_code)> OnDisconnectF;
        typedef boost::function<void(char const*, std::size_t)> OnReceiveF;

        virtual void on_connect(error_code /*ec*/) {}
        virtual void on_disconnect(error_code /*ec*/) {}
        virtual void on_receive(char const* /*data*/, std::size_t /*size*/) {}
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_TCP_PROTOCOL_HPP__