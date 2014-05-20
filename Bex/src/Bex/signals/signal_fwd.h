#ifndef __BEX_SIGNALS_SIGNAL_FWD__
#define __BEX_SIGNALS_SIGNAL_FWD__

#ifdef _MSC_VER
# pragma warning(disable:4996)
#endif //_MSC_VER

#include <list>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

namespace boost_signals2 = boost::signals2;

namespace Bex
{
    typedef boost::shared_ptr<boost_signals2::scoped_connection> signal_scoped;

    inline signal_scoped make_signal_scoped(boost_signals2::connection const& conn)
    {
        return signal_scoped(new boost_signals2::scoped_connection(conn));
    }
}

#endif //__BEX_SIGNALS_SIGNAL_FWD__