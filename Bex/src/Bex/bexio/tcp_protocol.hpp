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
        : ip::tcp
    {
        typedef buffered_socket<ip::tcp::socket, Buffer, Allocator> socket;
        typedef boost::shared_ptr<socket> socket_ptr;
        typedef Allocator allocator;
        typedef typename allocator::template rebind<socket>::other alloc_socket_t;

        // callback functions
        typedef boost::function<void(socket_ptr)> HandleSocketF;
        typedef boost::function<void(char const*, std::size_t)> OnReceiveF;

        static socket_ptr alloc_socket(io_service & ios)
        {
            return make_shared_ptr<socket, alloc_socket_t>(ios);
        }

    protected: 
        virtual void on_receive(char const* /*data*/, std::size_t /*size*/) {}

        void on_receive(const_buffer const& buffer)
        {
            on_receive(buffer_cast_helper(buffer), buffer_size_helper(buffer));
        }
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_TCP_PROTOCOL_HPP__