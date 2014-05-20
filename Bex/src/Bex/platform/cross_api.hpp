#ifndef __BEX_API__
#define __BEX_API__

#include <Bex/base.hpp>
#include <boost/thread.hpp>

namespace Bex
{
    inline void sys_sleep( uint64_t _Milli )
    {
        boost::this_thread::sleep( boost::posix_time::milliseconds(_Milli) );
    }
}

#endif //__BEX_API__