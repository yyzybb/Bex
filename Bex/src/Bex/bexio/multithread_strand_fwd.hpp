#ifndef __BEX_IO_MULTITHREAD_STRAND_FWD_HPP__
#define __BEX_IO_MULTITHREAD_STRAND_FWD_HPP__


#include "bexio_fwd.hpp"
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/identity.hpp>
#include "allocator.hpp"

namespace Bex { namespace bexio 
{
    template <class Strand = io_service, class Allocator = allocator>
    class multithread_strand;

    /// µ±Ç°²ãactor
    template <class T>
    io_service& actor(T & object)
    {
        return object.actor();
    }

    io_service& actor(io_service& object)
    {
        return object;
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_MULTITHREAD_STRAND_FWD_HPP__