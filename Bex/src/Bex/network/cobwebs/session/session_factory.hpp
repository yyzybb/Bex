#ifndef __BEX_NETWORK_COBWEBS_SESSION_FACTORY_HPP__
#define __BEX_NETWORK_COBWEBS_SESSION_FACTORY_HPP__

//////////////////////////////////////////////////////////////////////////
/// session»ùÀà

#include "session_base.hpp"
#include <Bex/network/cobwebs/core/deleter.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace Bex { namespace cobwebs
{
    template <class Session>
    class session_factory
    {
        BOOST_STATIC_ASSERT((boost::is_base_of<session_base, Session>::value));

    public:
        typedef Session session_type;
        typedef typename session_type::socket_type socket_type;
        typedef boost::shared_ptr<socket_type> socket_ptr;
        typedef boost::shared_ptr<Session> session_ptr;
        typedef session_ptr result_type;

        inline session_ptr operator()(socket_ptr pSock, session_initialized si)
        {
            session_ptr ptr(new Session(pSock, si), mainloop_deleter<Session>());
            ptr->initialize();
            return ptr;
        }
    };

    template <class ResultType, class Factory>
    boost::shared_ptr<ResultType> invoke_factory(Factory fact
        , typename Factory::socket_ptr pSock, session_initialized si)
    {
        BOOST_STATIC_ASSERT((boost::is_base_of<ResultType, typename Factory::session_type>::value));
        return boost::static_pointer_cast<ResultType>(fact(pSock, si));
    }

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_SESSION_FACTORY_HPP__