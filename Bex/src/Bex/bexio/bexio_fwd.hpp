#ifndef __BEX_IO_BEXIO_FWD_HPP__
#define __BEX_IO_BEXIO_FWD_HPP__

#include <Bex/config.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <list>
#include <pair>

namespace Bex { namespace bexio
{
    using namespace boost::asio;
    using boost::system::error_code;

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BEXIO_FWD_HPP__