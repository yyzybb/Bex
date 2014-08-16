#ifndef __BEX_IO_HTTP_HTTP_STREAM_IPP__
#define __BEX_IO_HTTP_HTTP_STREAM_IPP__

#include <boost/asio/yield.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <Bex/utility/format.hpp>

namespace Bex { namespace bexio { namespace http
{ 
    template <class Stream, class Allocator>
    void http_stream<Stream, Allocator>::open(url _url)
    {
        error_code ec;
        open(_url, ec);
        boost::asio::detail::throw_error(ec, "http_stream::open(url)");
    }

    template <class Stream, class Allocator>
    void http_stream<Stream, Allocator>::open(url _url, error_code & ec)
    {
        std::string host = proxy_ ? proxy_->hostname : _url.host();
        int port = proxy_ ? proxy_->port : _url.port();
        endpoint_type ep(ip::address::from_string(host, ec), port);

        if (ec)
        { // It is host.
            resolver slv(get_io_service());
            typename resolver::iterator first = slv.resolve(typename resolver::query(host, Bex::format("%d", port)), ec), last;
            if (ec)
                return ;

            while (first != last)
            {
                endpoint_type ep = (first++)->endpoint();
                std::string address = ep.address().to_string();
                lowest_layer().connect(ep, ec);
                if (!ec)
                    break;

                lowest_layer().close();
            }
        }
        else // It is ip address.
            lowest_layer().connect(ep, ec);

        if (ec) // All of the ip list cannot connect.
            return ;

        handshake(proxy_client, ec);
        if (ec)
            return ;

        request_header req = _url.to_request();

        // @Todo: Add universal options.
        // .i.e Connection, Accept, User-Agent, Accept-Encoding, Accept-Language
        std::string req_str = req.to_string();
        boost::asio::write(next_layer_, buffer(req_str.c_str(), req_str.length()), boost::asio::transfer_exactly(req_str.length()), ec);

        // Here, request send successfully!
    }

    template <class Stream, class Allocator>
    response_header http_stream<Stream, Allocator>::response()
    {
        error_code ec;
        response_header r = response(ec);
        boost::asio::detail::throw_error(ec);
        return r;
    }

    template <class Stream, class Allocator>
    response_header http_stream<Stream, Allocator>::response(error_code & ec)
    {
        response_header r;
        boost::asio::read_until(next_layer_, read_buffer_, option::header_end, ec);
        if (ec)
            return r;

        // create response header from string.
        std::size_t length = r.from_string((char const*)buffer_cast_helper(read_buffer_.data()));
        if (!length)
        {
            ec = make_error_code(errc::http_response_header_parse_error);
            return r;
        }

        // now, maybe has some data in read_buffer_.
        read_buffer_.consume(length);
        return r;
    }

    template <class Stream, class Allocator>
    void http_stream<Stream, Allocator>::handshake(handshake_type type)
    {
        error_code ec;
        handshake(type, ec);
        boost::asio::detail::throw_error(ec, "http_stream::handshake");
    }

    template <class Stream, class Allocator>
    void http_stream<Stream, Allocator>::handshake(handshake_type type, error_code & ec)
    {
        do_nextlayer_handshake<next_layer_type>(type, ec);
        if (ec) return ;

        do_proxy_handshake(type, ec);
    }

    template <class Stream, class Allocator>
    template <typename HandshakeHandler>
    void http_stream<Stream, Allocator>::async_handshake(handshake_type type, HandshakeHandler && handler)
    {
        do_async_nextlayer_handshake<next_layer_type>(type, std::forward<HandshakeHandler>(handler));
    }

    template <class Stream, class Allocator>
    void http_stream<Stream, Allocator>::shutdown()
    {
        boost::system::error_code ec;
        shutdown(ec);
        boost::asio::detail::throw_error(ec, "http_stream::shutdown");
    }

    template <class Stream, class Allocator>
    error_code http_stream<Stream, Allocator>::shutdown(error_code& ec)
    {
        return next_layer_.shutdown(ec);
    }

    template <class Stream, class Allocator>
    template <typename ShutdownHandler>
    void http_stream<Stream, Allocator>::async_shutdown(ShutdownHandler && handler)
    {
        next_layer_.async_shutdown(std::forward<ShutdownHandler>(handler));
    }

    template <class Stream, class Allocator>
    template <typename ConstBufferSequence>
    std::size_t http_stream<Stream, Allocator>::write_some(const ConstBufferSequence& buffers)
    {
        return next_layer_.write_some(buffers);
    }

    template <class Stream, class Allocator>
    template <typename ConstBufferSequence>
    std::size_t http_stream<Stream, Allocator>::write_some(const ConstBufferSequence& buffers, error_code & ec)
    {
        return next_layer_.write_some(buffers, ec);
    }

    template <class Stream, class Allocator>
    template <typename ConstBufferSequence, typename WriteHandler>
    void http_stream<Stream, Allocator>::async_write_some(const ConstBufferSequence& buffers, WriteHandler && handler)
    {
        next_layer_.async_write_some(buffers, std::forward<WriteHandler>(handler));
    }

    template <class Stream, class Allocator>
    template <typename MutableBufferSequence>
    std::size_t http_stream<Stream, Allocator>::read_some(const MutableBufferSequence& buffers)
    {
        if (read_buffer_.size())
        {
            std::size_t c = buffer_copy(buffers, read_buffer_.data());
            read_buffer_.consume(c);
            return c;
        }

        return next_layer_.read_some(buffers);
    }

    template <class Stream, class Allocator>
    template <typename MutableBufferSequence>
    std::size_t http_stream<Stream, Allocator>::read_some(const MutableBufferSequence& buffers, error_code & ec)
    {
        if (read_buffer_.size())
        {
            std::size_t c = buffer_copy(buffers, read_buffer_.data());
            read_buffer_.consume(c);
            return c;
        }

        return next_layer_.read_some(buffers, ec);
    }

    template <class Stream, class Allocator>
    template <typename MutableBufferSequence, typename ReadHandler>
    void http_stream<Stream, Allocator>::async_read_some(const MutableBufferSequence& buffers, ReadHandler && handler)
    {
        if (read_buffer_.size())
        {
            std::size_t c = buffer_copy(buffers, read_buffer_.data());
            read_buffer_.consume(c);
            error_code ec;
            handler(ec, c);
            return ;
        }

        next_layer_.async_read_some(buffers, std::forward<ReadHandler>(handler));
    }

    template <class Stream, class Allocator>
    template <typename NextLayerType>
        typename boost::enable_if<has_handshake_stream<NextLayerType>>::type
        http_stream<Stream, Allocator>::do_nextlayer_handshake(handshake_type type, error_code & ec)
    {
        next_layer_.handshake(type, ec);
        if (ec)
            return ;

        do_proxy_handshake(type, ec);
    }

    template <class Stream, class Allocator>
    template <typename NextLayerType>
        typename boost::disable_if<has_handshake_stream<NextLayerType>>::type
        http_stream<Stream, Allocator>::do_nextlayer_handshake(handshake_type type, error_code & ec)
    {
        do_proxy_handshake(type, ec);
    }

    template <class Stream, class Allocator>
    void http_stream<Stream, Allocator>::do_proxy_handshake(handshake_type type, error_code & ec)
    {
        // @Todo: Use coroutine to perform proxy handshake logic.
        ec.clear();
    }

    template <class Stream, class Allocator>
    template <typename NextLayerType, typename HandshakeHandler>
        typename boost::enable_if<has_async_handshake_stream<NextLayerType>>::type
    http_stream<Stream, Allocator>::do_async_nextlayer_handshake(handshake_type type, HandshakeHandler && handler)
    {
        next_layer_.async_handshake(type
            , BEX_IO_BIND(&http_stream<Stream, Allocator>::on_nextlayer_handshake<HandshakeHandler>
                , this, _1, std::forward<HandshakeHandler>(handler), get_token()));
    }

    template <class Stream, class Allocator>
    template <typename NextLayerType, typename HandshakeHandler>
        typename boost::disable_if<has_async_handshake_stream<NextLayerType>>::type
    http_stream<Stream, Allocator>::do_async_nextlayer_handshake(handshake_type type, HandshakeHandler && handler)
    {
        error_code ec;
        on_nextlayer_handshake(ec, std::forward<HandshakeHandler>(handler), get_token());
    }

    template <class Stream, class Allocator>
    template <typename HandshakeHandler>
    void http_stream<Stream, Allocator>::do_async_proxy_handshake(handshake_type type, HandshakeHandler && handler, life_token_type)
    {
        // @Todo: Use coroutine to perform proxy handshake logic.
        error_code ec;
        handler(ec);
    }

    template <class Stream, class Allocator>
    template <typename HandshakeHandler>
    void http_stream<Stream, Allocator>::on_nextlayer_handshake(error_code const& ec, handshake_type type, HandshakeHandler & handler, life_token_type)
    {
        if (ec || !proxy_)
            handler(ec);
        else
            do_async_proxy_handshake(type
                , BEX_IO_BIND(&http_stream::on_proxy_handshake<HandshakeHandler>, this, _1, handler, get_token()));
    }

    template <class Stream, class Allocator>
    template <typename HandshakeHandler>
    void http_stream<Stream, Allocator>::on_proxy_handshake(error_code const& ec, HandshakeHandler & handler, life_token_type)
    {
        handler(ec);
    }

    template <class Stream, class Allocator>
    typename http_stream<Stream, Allocator>::life_token_type http_stream<Stream, Allocator>::get_token()
    {
        return ::boost::enable_shared_from_this<this_type>::shared_from_this();
    }

} //namespace http
} //namespace bexio
} //namespace Bex

#include <boost/asio/unyield.hpp>

#endif //__BEX_IO_HTTP_HTTP_STREAM_IPP__
