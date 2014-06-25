#ifndef __BEX_IO_BUFFERED_SOCKET_HPP__
#define __BEX_IO_BUFFERED_SOCKET_HPP__

//////////////////////////////////////////////////////////////////////////
/// socket之上封装一层缓冲区
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    template <typename Socket, typename Buffer, typename Allocator>
    class buffered_socket
    {
    public:
        buffered_socket(io_service & ios)
            : socket_(ios)
        {}


    private:
        Socket socket_;
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BUFFERED_SOCKET_HPP__