#include "id.h"
#include <boost/thread/mutex.hpp>

namespace Bex { namespace cobwebs
{
#if !defined(BEX_SUPPORT_CXX11)
    volatile long id32::s_id = 1;
    boost::uint64_t id64::s_id = 1;
#else
    std::atomic<long> id32::s_id {1};
    std::atomic<long long> id64::s_id {1};
#endif

    static boost::mutex s_mu;
    boost::uint64_t id64::create()
    {
#if !defined(BEX_SUPPORT_CXX11)
        boost::mutex::scoped_lock lock(s_mu);
#endif 
        return s_id++;
    } 

    id64 create_id64( boost::uint64_t id_ )
    {
        return id64(id_);
    }

    id32 create_id32( boost::uint32_t id_ )
    {
        return id32(id_);
    }

} //namespace cobwebs
} //namespace Bex