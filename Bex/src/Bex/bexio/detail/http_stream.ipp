#ifndef __BEX_IO_HTTP_HTTP_STREAM_IPP__
#define __BEX_IO_HTTP_HTTP_STREAM_IPP__

#include <boost/asio/yield.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>

namespace Bex { namespace bexio { namespace http
{ 
    template <class Stream>
    void http_stream<Stream>::open(url _url)
    {
        error_code ec;
        open(_url, ec);
        boost::asio::detail::throw_error(ec, "http_stream::open(url)");
    }

    template <class Stream>
    void http_stream<Stream>::open(url _url, error_code & ec)
    {
        std::string host = proxy_ ? proxy_->hostname : _url.host();
        int port = proxy_ ? proxy_->port : _url.port();
        endpoint_type ep(ip::address::from_string(host, ec), port);

        if (ec)
        { // It is host.
            resolver slv(get_io_service());
            typename resolver::iterator first = slv.resolve(typename resolver::query(protocol_type::v4().procotol(), host)), last;
            while (first != last)
            {
                endpoint_type ep = (first++)->endpoint();
                lowest_layer().connect(ep, ec);
                if (!ec)
                    break;
            }
        }
        else // It is ip address.
            lowest_layer().connect(ep, ec);

        if (ec)     // All of the ip list cannot connect.
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

    template <class Stream>
    response_header http_stream<Stream>::response()
    {
        error_code ec;
        boost::asio::read_until(next_layer_, synchronous_buffer_, option::header_end, ec);
        // @Todo: create response and save other data in synchronous.
        // synchronous operation also use buffer_.
    }

    template <class Stream>
    void http_stream<Stream>::handshake(handshake_type type)
    {
        error_code ec;
        handshake(type, ec);
        boost::asio::detail::throw_error(ec, "http_stream::handshake");
    }

    template <class Stream>
    void http_stream<Stream>::handshake(handshake_type type, error_code & ec)
    {
        do_nextlayer_handshake(type, ec);
        if (ec) return ;

        do_proxy_handshake(type, ec);
    }

    template <class Stream>
    template <typename HandshakeHandler>
    void http_stream<Stream>::async_handshake(handshake_type type, HandshakeHandler && handler)
    {
        do_async_nextlayer_handshake(type, std::forward<HandshakeHandler>(handler));
    }

    template <class Stream>
    void http_stream<Stream>::shutdown()
    {
        boost::system::error_code ec;
        shutdown(ec);
        boost::asio::detail::throw_error(ec, "http_stream::shutdown");
    }

    template <class Stream>
    error_code http_stream<Stream>::shutdown(error_code& ec)
    {
        return next_layer_.shutdown(ec);
    }

    template <class Stream>
    template <typename ShutdownHandler>
    void http_stream<Stream>::async_shutdown(ShutdownHandler && handler)
    {
        next_layer_.async_shutdown(std::forward<ShutdownHandler>(handler));
    }

    template <class Stream>
    template <typename ConstBufferSequence>
    std::size_t http_stream<Stream>::write_some(const ConstBufferSequence& buffers)
    {
        next_layer_.write_some(buffers);
    }

    template <class Stream>
    template <typename ConstBufferSequence>
    std::size_t http_stream<Stream>::write_some(const ConstBufferSequence& buffers, error_code & ec)
    {
        next_layer_.write_some(buffers, ec);
    }

    template <class Stream>
    template <typename ConstBufferSequence, typename WriteHandler>
    void http_stream<Stream>::async_write_some(const ConstBufferSequence& buffers, WriteHandler && handler)
    {
        next_layer_.async_write_some(buffers, std::forward<WriteHandler>(handler));
    }

    template <class Stream>
    template <typename MutableBufferSequence>
    std::size_t http_stream<Stream>::read_some(const MutableBufferSequence& buffers)
    {
        next_layer_.read_some(buffers);
    }

    template <class Stream>
    template <typename MutableBufferSequence>
    std::size_t http_stream<Stream>::read_some(const MutableBufferSequence& buffers, error_code & ec)
    {
        next_layer_.read_some(buffers, ec);
    }

    template <class Stream>
    template <typename MutableBufferSequence, typename ReadHandler>
    void http_stream<Stream>::async_read_some(const MutableBufferSequence& buffers, ReadHandler && handler)
    {
        next_layer_.async_read_some(buffers, std::forward<ReadHandler>(handler));
    }

    template <class Stream>
    template <typename /*= int*/>
    typename boost::enable_if<has_handshake_stream<typename http_stream<Stream>::next_layer_type>, void>::type
        http_stream<Stream>::do_nextlayer_handshake(handshake_type type, error_code & ec)
    {
        next_layer_.handshake(type, ec);
    }

    template <class Stream>
    template <typename /*= int*/>
    typename boost::disable_if<has_handshake_stream<typename http_stream<Stream>::next_layer_type>, void>::type
        http_stream<Stream>::do_nextlayer_handshake(handshake_type, error_code & ec)
    {
        ec.clear();
    }

    template <class Stream>
    template <typename HandshakeHandler>
    typename boost::enable_if<has_async_handshake_stream<typename http_stream<Stream>::next_layer_type>, void>::type
        http_stream<Stream>::do_async_nextlayer_handshake(handshake_type type, HandshakeHandler && handler)
    {
        next_layer_.async_handshake(type
            , BEX_IO_BIND(&http_stream<Stream>::on_nextlayer_handshake<HandshakeHandler>
                , this, _1, std::forward<HandshakeHandler>(handler), get_token()));
    }

    template <class Stream>
    template <typename HandshakeHandler>
    typename boost::disable_if<has_async_handshake_stream<typename http_stream<Stream>::next_layer_type>, void>::type
        http_stream<Stream>::do_async_nextlayer_handshake(handshake_type type, HandshakeHandler && handler)
    {
        error_code ec;
        on_nextlayer_handshake(ec, std::forward<HandshakeHandler>(handler), get_token());
    }

    template <class Stream>
    template <typename HandshakeHandler>
    void http_stream<Stream>::do_async_proxy_handshake(handshake_type type, HandshakeHandler && handler, life_token_type)
    {
        // @Todo: Use coroutine to perform proxy handshake logic.
        error_code ec;
        handler(ec);
    }

    template <class Stream>
    template <typename HandshakeHandler>
    void http_stream<Stream>::on_nextlayer_handshake(error_code const& ec, handshake_type type, HandshakeHandler & handler, life_token_type)
    {
        if (ec || !proxy_)
            handler(ec);
        else
            do_async_proxy_handshake(type
                , BEX_IO_BIND(&http_stream::on_proxy_handshake<HandshakeHandler>, this, _1, handler, get_token()));
    }

    template <class Stream>
    template <typename HandshakeHandler>
    void http_stream<Stream>::on_proxy_handshake(error_code const& ec, HandshakeHandler & handler, life_token_type)
    {
        handler(ec);
    }

    template <class Stream>
    typename http_stream<Stream>::life_token_type http_stream<Stream>::get_token()
    {
        return ::boost::enable_shared_from_this<this_type>::shared_from_this();
    }

} //namespace http
} //namespace bexio
} //namespace Bex

#include <boost/asio/unyield.hpp>

#endif //__BEX_IO_HTTP_HTTP_STREAM_IPP__
