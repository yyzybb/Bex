#ifndef __BEX_IO_BEXIO_FWD_HPP__
#define __BEX_IO_BEXIO_FWD_HPP__

#include <Bex/config.hpp>
#include <Bex/thread/lock/inter_lock.hpp>
#include <Bex/type_traits/class_info.hpp>
#include <Bex/utility/singleton.hpp>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/array.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/operators.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/mpl/print.hpp>
#include <boost/typeof/typeof.hpp>
#include <numeric>
#include <list>
#include <atomic>
#include <type_traits>
#include "options.hpp"
#include "error.hpp"
#include "sentry.hpp"
#include "allocator.hpp"

#if defined(BEX_USE_FORWARDBIND)
#include <Bex/bind.hpp>
# define BEX_IO_BIND ::Bex::forward_bind::BEX_BIND
# define BEX_IO_PH_ERROR ::Bex::_1
# define BEX_IO_PH_BYTES_TRANSFERRED ::Bex::_2
using ::Bex::_1;
using ::Bex::_2;
using ::Bex::_3;
using ::Bex::_4;
using ::Bex::_5;
using ::Bex::_6;
#else
# define BEX_IO_BIND ::boost::BOOST_BIND
# define BEX_IO_PH_ERROR ::boost::asio::placeholders::error
# define BEX_IO_PH_BYTES_TRANSFERRED ::boost::asio::placeholders::bytes_transferred
#endif 

namespace Bex { namespace bexio
{
    using namespace boost::asio;
    using boost::system::error_code;
    using boost::shared_ptr;
    using boost::weak_ptr;
    using detail::buffer_cast_helper;
    using detail::buffer_size_helper;

    //////////////////////////////////////////////////////////////////////////
    /// @{ meta functions
    /// 类型定义concept检测
    template <class T>
    struct has_next_layer
    {
        template <typename U>
        static char _check(...);

        template <typename U>
        static short _check(typename U::next_layer_type*);

        static const bool value = (sizeof(_check<T>(0)) == sizeof(short));
    };

    template <class T>
    struct has_lowest_layer
    {
        template <typename U>
        static char _check(...);

        template <typename U>
        static short _check(typename U::lowest_layer_type*);

        static const bool value = (sizeof(_check<T>(0)) == sizeof(short));
    };

    /// 分层类型中的下一层类型
    template <class T>
    typename T::next_layer_type& next_layer(T& object)
    {
        return object.next_layer();
    }

    template <class T>
    typename boost::disable_if<has_next_layer<T>, T&>::type next_layer(T& object, ...)
    {
        return object;
    }

    /// 分层类型中的最底层类型
    template <class T>
    typename T::lowest_layer_type& lowest_layer(T& object)
    {
        return object.lowest_layer();
    }

    template <class T>
    typename boost::disable_if<has_lowest_layer<T>, T&>::type lowest_layer(T& object, ...)
    {
        return object;
    }

    /// 萃取分层类型中的下一层类型
    template <class T, bool>
    struct next_layer_t_helper
        : boost::mpl::identity<typename T::next_layer_type>
    {};

    template <class T>
    struct next_layer_t_helper<T, false>
        : boost::mpl::identity<T>
    {};

    template <class T>
    struct next_layer_t
        : next_layer_t_helper<T, has_next_layer<T>::value >
    {};

    /// 萃取分层类型中的最底层类型
    template <class T, bool>
    struct lowest_layer_t_helper
        : boost::mpl::identity<typename T::lowest_layer_type>
    {};

    template <class T>
    struct lowest_layer_t_helper<T, false>
        : boost::mpl::identity<T>
    {};

    template <class T>
    struct lowest_layer_t
        : lowest_layer_t_helper<T, has_lowest_layer<T>::value >
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BEXIO_FWD_HPP__
