#ifndef __BEX_NETWORK_COBWEBS_REGISTER_HPP__
#define __BEX_NETWORK_COBWEBS_REGISTER_HPP__

//////////////////////////////////////////////////////////////////////////

#include "core.h"

namespace Bex { namespace cobwebs
{
    class CRegisterCore
    {
        typedef boost::function<void()> RunHandler;
        signal_scoped m_ss;

    public:
        void set(RunHandler const& handler, int group)
        {
            m_ss = core::getInstance().register_run(handler, group);
        }

        void cancel()
        {
            m_ss.reset();
        }
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_REGISTER_HPP__