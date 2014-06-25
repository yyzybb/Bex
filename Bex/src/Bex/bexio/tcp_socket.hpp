#ifndef __BEX_IO_TCP_SOCKET_HPP__
#define __BEX_IO_TCP_SOCKET_HPP__

//////////////////////////////////////////////////////////////////////////
/// tcp socket policy
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    template <typename Protocol, 
        typename Allocator,
        template <typename Socket, typename Alloc> class Buffered
    >
    struct tcp_socket_policy
    {
        typedef typename Protocol::socket socket;
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_TCP_SOCKET_HPP__