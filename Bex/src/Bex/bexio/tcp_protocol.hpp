#ifndef __BEX_IO_TCP_PROTOCOL_HPP__
#define __BEX_IO_TCP_PROTOCOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// tcp socket policy
#include "bexio_fwd.hpp"
#include "buffered_socket.hpp"
#include "nonblocking_circularbuffer.hpp"
#include "allocator.hpp"

namespace Bex { namespace bexio
{
    template <typename Buffer = nonblocking_circularbuffer,
        typename Allocator = ::Bex::bexio::allocator<int> >
    struct tcp_protocol
    {
        typedef ip::tcp::endpoint endpoint;
        typedef ip::tcp::acceptor acceptor;
        typedef ip::tcp::resolver resolver;
        typedef buffered_socket<ip::tcp::socket, Buffer, Allocator> socket;
        typedef boost::shared_ptr<socket> socket_ptr;
        typedef Allocator allocator;
        typedef typename allocator::template rebind<socket>::other alloc_socket_t;

        // callback functions
        //typedef boost::function<void(socket_ptr)> HandleSocketF;
        typedef boost::function<void(char const*, std::size_t)> OnReceiveF;

        static socket_ptr alloc_socket(io_service & ios, std::size_t rbsize, std::size_t wbsize)
        {
            return make_shared_ptr<socket, alloc_socket_t>(ios, rbsize, wbsize);
        }

    protected: 
        virtual void on_receive(char const* /*data*/, std::size_t /*size*/) {}

        template <typename F, typename Id>
        inline void parse(F f, Id const& id, const_buffer const& buffer)
        {
            f(id, (char const*)buffer_cast_helper(buffer), buffer_size_helper(buffer));
        }

        inline void parse(const_buffer const& buffer)
        {
            on_receive( (char const*)buffer_cast_helper(buffer), buffer_size_helper(buffer));
        }

        void on_receive(const_buffer const& buffer)
        {
            parse(buffer);
        }
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_TCP_PROTOCOL_HPP__