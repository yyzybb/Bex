#ifndef __BEX_NETWORK_COBWEBS_CONNECTOR_BASE_H__
#define __BEX_NETWORK_COBWEBS_CONNECTOR_BASE_H__

//////////////////////////////////////////////////////////////////////////
/// Á¬½ÓÆ÷

#include <Bex/network/cobwebs/core/core.h>
#include <Bex/network/cobwebs/session/session_base.hpp>

namespace Bex { namespace cobwebs
{
    class connector_base
    {
    protected:
        io_service& m_ios;

    public:
        connector_base()
            : m_ios(core::getInstance().get_io_service())
        {
        }
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_CONNECTOR_BASE_H__