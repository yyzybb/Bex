#ifndef __BEX_IO_HTTP_HTTP_STREAM_HPP__
#define __BEX_IO_HTTP_HTTP_STREAM_HPP__

//////////////////////////////////////////////////////////////////////////
/// http stream
#include <Bex/bexio/bexio_fwd.hpp>
#include "http_options.hpp"
#include "url.hpp"

namespace Bex { namespace bexio { namespace http
{ 
    template <class Stream>
    class http_stream
    {
    public:
        enum class proxy_handshake_type {
            proxy_client,
            proxy_server,
        };

        typedef typename boost::remove_reference<Stream>::type next_layer_type;
        typedef typename lowest_layer_t<next_layer_type>::type lowest_layer_type;

        template <typename ... Args>
        explicit http_stream(Args && ... args)
            : next_layer_(std::forward<Args>(args)...)
        {}

        /// Access an url, it maybe throw exception.
        void open(url _url);

        /// Access an url, it's noexcept function.
        void open(url _url, error_code & ec);

        boost::asio::io_service& get_io_service()
        {
            return ::Bex::bexio::lowest_layer(next_layer_).get_io_service();
        }

        const next_layer_type& next_layer() const
        {
            return next_layer_;
        }

        next_layer_type& next_layer()
        {
            return next_layer_;
        }

        lowest_layer_type& lowest_layer()
        {
            return ::Bex::bexio::lowest_layer(next_layer_);
        }

        /// Perform Http proxy procotol handshaking, it maybe throw exception.
        /**
         * If the next_layer is SSL stream, it will perform SSL handshake first.
         * If the proxy has been set, it will perform Http proxy procotol handshake.
         * Otherwise, the function do nothing.
         */
        void handshake(proxy_handshake_type type);

        /// Perform Http proxy procotol handshaking, it's noexcept function.
        /**
         * If the next_layer is SSL stream, it will perform SSL handshake first.
         * If the proxy has been set, it will perform Http proxy procotol handshake.
         * Otherwise, the function do nothing.
         */
        void handshake(proxy_handshake_type type, error_code & ec);

        /// Start an asynchronous handshake.
        /**
         * If the next_layer is SSL stream, it will perform SSL handshake first.
         * If the proxy has been set, it will perform Http proxy procotol handshake.
         * Otherwise, the function do nothing.
         *
         * @param type The type of handshaking to be performed, i.e. as a proxy_client or as a proxy_server.
         *
         * @param handler The handler to be called when the handshake operation
         * completes. Copies will be made of the handler as required. The equivalent
         * function signature of the handler must be:
         * @code void handler(
         *   const boost::system::error_code& error // Result of operation.
         * ); @endcode
         */
        template <typename HandshakeHandler>
            BOOST_ASIO_INITFN_RESULT_TYPE(HandshakeHandler, void (boost::system::error_code))
        async_handshake(proxy_handshake_type type, HandshakeHandler && handler);

        /// Shutdown the http stream, it maybe throw exception.
        /**
         * The function is used to perform next_layer::async_shutdown.
         */
        void shutdown();

        /// Shutdown the http stream, it's noexcept function.
        /**
         * The function is used to perform next_layer::async_shutdown.
         */
        error_code shutdown(error_code& ec);

        /// Asynchronously shut down SSL on the stream.
        /**
         * 
         * The function is used to perform next_layer::async_shutdown.
         *
         * @param handler The handler to be called when the handshake operation
         * completes. Copies will be made of the handler as required. The equivalent
         * function signature of the handler must be:
         * @code void handler(
         *   const boost::system::error_code& error // Result of operation.
         * ); @endcode
         */
        template <typename ShutdownHandler>
            BOOST_ASIO_INITFN_RESULT_TYPE(ShutdownHandler, void (boost::system::error_code))
        async_shutdown(ShutdownHandler && handler);

        /// Write some data to the stream, it maybe throw exception.
        template <typename ConstBufferSequence>
        std::size_t write_some(const ConstBufferSequence& buffers);

        /// Write some data to the stream, it's noexcept function.
        template <typename ConstBufferSequence>
        std::size_t write_some(const ConstBufferSequence& buffers, error_code & ec);

        /// Start an asynchronous write.
        template <typename ConstBufferSequence, typename WriteHandler>
            BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t))
        async_write_some(const ConstBufferSequence& buffers, WriteHandler && handler);

        /// Read some data to the stream, it maybe throw exception.
        template <typename ConstBufferSequence>
        std::size_t read_some(const ConstBufferSequence& buffers);

        /// Read some data to the stream, it's noexcept function.
        template <typename ConstBufferSequence>
        std::size_t read_some(const ConstBufferSequence& buffers, error_code & ec);

        /// Start an asynchronous read.
        template <typename ConstBufferSequence, typename WriteHandler>
            BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t))
        async_read_some(const ConstBufferSequence& buffers, WriteHandler && handler);

    private:
        /// Perform nextlayer handshake
        template <typename HandshakeHandler>
        decltype(std::declval(next_layer_type).handshake(0, std::declval(error_code)))
        do_nextlayer_handshake(proxy_handshake_type type, error_code & ec);

        /// Start an asynchronous nextlayer handshake
        template <typename HandshakeHandler>
        void do_async_nextlayer_handshake(proxy_handshake_type type, HandshakeHandler && handler);

        /// Start an asynchronous proxy handshake
        template <typename HandshakeHandler>
        void do_async_proxy_handshake(proxy_handshake_type type, HandshakeHandler && handler);

        /// next_layer asynchronous handshake handler
        template <typename HandshakeHandler>
        void on_nextlayer_handshake(error_code const& ec, HandshakeHandler & handler);

        /// http proxy protocol asynchronous handshake handler
        template <typename HandshakeHandler>
        void on_proxy_handshake(error_code const& ec, HandshakeHandler & handler);

    private:
        Stream next_layer_;
    };

} //namespace http
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_HTTP_HTTP_STREAM_HPP__