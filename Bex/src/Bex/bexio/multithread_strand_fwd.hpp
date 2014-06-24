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

    /// 分层类型中的最底层类型
    template <class T> 
    typename T::next_layer_type& next_layer(T& object)
    {
        return object.next_layer();
    }

    io_service& next_layer(io_service& object)
    {
        return object;
    }

    /// 分层类型中的最底层类型
    template <class T> 
    typename T::lowest_layer_type& lowest_layer(T& object)
    {
        return object.lowest_layer();
    }

    io_service& lowest_layer(io_service& object)
    {
        return object;
    }

    /// 当前层actor
    template <class T>
    io_service& actor(T & object)
    {
        return object.actor();
    }

    io_service& actor(io_service& object)
    {
        return object;
    }

    /// 萃取分层类型中的最底层类型
    template <class T>
    struct lowest_layer_type
    {
        typedef typename T::lowest_layer_type type;
    };

    template <>
    struct lowest_layer_type<io_service> : boost::mpl::identity<io_service> {};

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_MULTITHREAD_STRAND_FWD_HPP__