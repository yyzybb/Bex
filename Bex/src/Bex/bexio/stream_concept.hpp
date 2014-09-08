#ifndef __BEX_IO_STREAM_CONCEPT_HPP__
#define __BEX_IO_STREAM_CONCEPT_HPP__

#include <boost/system/error_code.hpp>

namespace Bex { namespace bexio
{ 
    /// Check a stream type has handshake interface.
    template <class Stream>
    struct has_handshake_stream
    {
        template <typename U>
        static char _check(U *
            , decltype(std::declval<U>().handshake((typename Stream::handshake_type)0, boost::throws()))* = nullptr);

        template <typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Stream>(nullptr)) == sizeof(char));
    };

    /// Check a stream type has asynchronous handshake interface.
    template <class Stream>
    struct has_async_handshake_stream
    {
        static void _handler(boost::system::error_code) {}

        template <typename U>
        static char _check(U *
            , decltype(std::declval<U>().async_handshake((typename Stream::handshake_type)0, &_handler))* = nullptr);

        template <typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Stream>(nullptr)) == sizeof(char));
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_STREAM_CONCEPT_HPP__
