#ifndef __BEX_IO_SSL_PROTOCOL_HPP__
#define __BEX_IO_SSL_PROTOCOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// ssl–≠“È
#include "bexio_fwd.hpp"
#include <boost/asio/ssl.hpp>

namespace Bex { namespace bexio
{
    template <typename NextLayerProtocol, 
        ssl::context_base::method SSLMethod = ssl::context_base::sslv23>
    struct ssl_protocol
        : public NextLayerProtocol
    {
        typedef typename NextLayerProtocol::native_socket_type nextlayer_socket_type;
        typedef typename NextLayerProtocol::buffer_type buffer_type;
        typedef typename NextLayerProtocol::allocator allocator;

        struct native_socket_type
            : private ssl::context
            , public ssl::stream<layer_socket_type>
        {
            typedef ssl::context ctx_type;
            typedef ssl::stream<layer_socket_type> stream_type;

            explicit native_socket_type(io_service & ios)
                : ctx_type(ios, SSLMethod)
                , stream_type(ios, static_cast<ctx_type&>(*this))
                , strand_(ios)
            {}

            template <typename MutableBufferSequence, typename ReadHandler>
            void async_read_some(const MutableBufferSequence& buffers,
                ReadHandler const& handler)
            {
                stream_type.async_read_some(buffers, strand_.wrap(handler));
            }


            template <typename ConstBufferSequence, typename WriteHandler>
            void async_write_some(const ConstBufferSequence& buffers,
                WriteHandler const& handler)
            {
                stream_type.async_write_some(buffers, strand_.wrap(handler));
            }

            strand strand_;
        };

        typedef buffered_socket<native_socket_type, buffer_type, allocator> socket;
        typedef shared_ptr<socket> socket_ptr;
        typedef ssl::stream_base::handshake_type handshake_type;

        static socket_ptr alloc_socket(io_service & ios, std::size_t rbsize, std::size_t wbsize)
        {
            return make_shared_ptr<socket, allocator>(ios, rbsize, wbsize);
        }

        template <typename Handler>
        static void async_shutdown(socket_ptr sp, BEX_MOVE_ARG(Handler) handler)
        {
            sp->next_layer().async_shutdown(BEX_MOVE_CAST(Handler)(handler));
        }

        template <typename Handler>
        static void async_handshake(socket_ptr sp, handshake_type hstype, BEX_MOVE_ARG(Handler) handler)
        {
            sp->next_layer().async_handshake(hstype, BEX_MOVE_CAST(Handler)(handler));
        }
    };


} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SSL_PROTOCOL_HPP__