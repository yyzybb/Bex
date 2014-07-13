#ifndef __BEX_IO_SSL_PROTOCOL_HPP__
#define __BEX_IO_SSL_PROTOCOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// ssl–≠“È
#include "bexio_fwd.hpp"
#include <boost/asio/ssl.hpp>

#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libeay32.lib")

namespace Bex { namespace bexio
{
    template <typename NextLayerProtocol, 
        ssl::context_base::method SSLMethod = ssl::context_base::sslv23>
    struct ssl_protocol
        : public NextLayerProtocol
    {
        typedef NextLayerProtocol next_layer_type;
        typedef typename lowest_layer_t<NextLayerProtocol>::type lowest_layer_type;

        typedef typename NextLayerProtocol::native_socket_type nextlayer_socket_type;
        typedef typename NextLayerProtocol::buffer_type buffer_type;
        typedef typename NextLayerProtocol::allocator allocator;

        struct native_socket_t
            : public ssl::stream<nextlayer_socket_type>
        {
            typedef ssl::context ctx_type;
            typedef shared_ptr<ctx_type> ctx_ptr;
            typedef ssl::stream<nextlayer_socket_type> stream_type;

            explicit native_socket_t(io_service & ios, ctx_ptr cp)
                : stream_type(ios, *cp)
                , strand_(ios)
                , ctx_(cp)
            {}

            template <typename MutableBufferSequence, typename ReadHandler>
            void async_read_some(const MutableBufferSequence& buffers,
                ReadHandler const& handler)
            {
                stream_type::async_read_some(buffers, strand_.wrap(handler));
            }


            template <typename ConstBufferSequence, typename WriteHandler>
            void async_write_some(const ConstBufferSequence& buffers,
                WriteHandler const& handler)
            {
                stream_type::async_write_some(buffers, strand_.wrap(handler));
            }

            ctx_type & context()
            {
                return static_cast<ctx_type&>(*ctx_);
            }

            strand strand_;
            ctx_ptr ctx_;
        };

        typedef buffered_socket<native_socket_t, buffer_type, allocator> socket;
        typedef shared_ptr<socket> socket_ptr;
        typedef ssl::stream_base::handshake_type handshake_type;
        typedef native_socket_t native_socket_type;

        static socket_ptr alloc_socket(io_service & ios, options & opts, error_code & ec)
        {
            shared_ptr<ssl::context> ctx = make_shared_ptr<ssl::context, allocator>(ios, SSLMethod);

            ctx->set_options(opts.ssl_opts.ctx_opt, ec);
            if (ec)
                return socket_ptr();

            ctx->set_verify_mode(opts.ssl_opts.verify_mode, ec);
            if (ec)
                return socket_ptr();

            if (!opts.ssl_opts.crt_file.empty())
            {
                ctx->use_certificate_chain_file(opts.ssl_opts.crt_file, ec);
                if (ec)
                    return socket_ptr();
            }

            if (!opts.ssl_opts.pri_key_file.empty())
            {
                ctx->use_private_key_file(opts.ssl_opts.pri_key_file, opts.ssl_opts.file_fmt, ec);
                if (ec)
                    return socket_ptr();
            }

            if (!opts.ssl_opts.dh_file.empty())
            {
                ctx->use_tmp_dh_file(opts.ssl_opts.dh_file, ec);
                if (ec)
                    return socket_ptr();
            }

            if (!opts.ssl_opts.cipher_list.empty())
                SSL_CTX_set_cipher_list(ctx->native_handle(), opts.ssl_opts.cipher_list.c_str());

            socket_ptr sp = make_shared_ptr<socket, allocator>(ios
                , ctx, opts.receive_buffer_size, opts.send_buffer_size);

            ec.clear();
            return sp;
        }

        struct invoker
        {
            template <typename Handler>
            void async_shutdown(socket_ptr sp, BEX_MOVE_ARG(Handler) handler)
            {
                sp->next_layer().async_shutdown(BEX_MOVE_CAST(Handler)(handler));
            }

            template <typename Handler>
            void async_handshake(socket_ptr sp, handshake_type hstype, BEX_MOVE_ARG(Handler) handler)
            {
                sp->next_layer().async_handshake(hstype, BEX_MOVE_CAST(Handler)(handler));
            }

            void handshake(socket_ptr sp, handshake_type hstype, error_code & ec)
            {
                sp->next_layer().handshake(hstype, ec);
            }
        };
    };


} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SSL_PROTOCOL_HPP__