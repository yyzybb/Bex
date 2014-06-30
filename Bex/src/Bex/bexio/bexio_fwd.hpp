#ifndef __BEX_IO_BEXIO_FWD_HPP__
#define __BEX_IO_BEXIO_FWD_HPP__

#include <Bex/config.hpp>
#include <Bex/thread/lock/inter_lock.hpp>
#include <Bex/type_traits/class_info.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/array.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/operators.hpp>
#include <boost/tuple/tuple.hpp>
#include <numeric>
#include <list>
#include "options.hpp"
#include "error.hpp"
#include "sentry.hpp"

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

    /// boost::function addition
    template <typename Addition, typename F>
    struct function_addition;

    template <typename Addition, typename R>
    struct function_addition<Addition, boost::function<R()> >
        : boost::mpl::identity<boost::function<R(Addition)> >
    {};

    template <typename Addition, typename R, typename Arg1>
    struct function_addition<Addition, boost::function<R(Arg1)> >
        : boost::mpl::identity<boost::function<R(Addition, Arg1)> >
    {};

    template <typename Addition, typename R, typename Arg1, typename Arg2>
    struct function_addition<Addition, boost::function<R(Arg1, Arg2)> >
        : boost::mpl::identity<boost::function<R(Addition, Arg1, Arg2)> >
    {};

    template <typename Addition, typename R, typename Arg1, typename Arg2, typename Arg3>
    struct function_addition<Addition, boost::function<R(Arg1, Arg2, Arg3)> >
        : boost::mpl::identity<boost::function<R(Addition, Arg1, Arg2, Arg3)> >
    {};

    /// post strand
    template <typename Protocol>
    struct has_post_strand
    {
        struct Handler {};

        template <typename U,
            detail::wrapped_handler<io_service::strand, Handler, detail::is_continuation_if_running>(U::*)(BOOST_ASIO_MOVE_ARG(Handler))>
        struct impl;

        template <typename U>
        static char _check(U*, impl<U, (&U::template post_strand<Handler>) >*);

        template <typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol>(0, 0)) == sizeof(char));
    };

    template <typename Protocol, typename Handler, bool has>
    struct post_strand_helper
    {
        typedef detail::wrapped_handler<io_service::strand, Handler, detail::is_continuation_if_running> const& result_type;
        result_type operator()(Protocol & proto, BOOST_ASIO_MOVE_ARG(Handler) handler)
        {
            return proto.post_strand(BOOST_ASIO_MOVE_CAST(Handler)(handler));
        }
    };

    template <typename Protocol, typename Handler>
    struct post_strand_helper<Protocol, Handler, false>
    {
        typedef Handler const& result_type;
        result_type operator()(Protocol &, BOOST_ASIO_MOVE_ARG(Handler) handler)
        {
            return BOOST_ASIO_MOVE_CAST(Handler)(handler);
        }
    };

    template <typename Protocol, typename Handler>
    struct post_strand_c
        : post_strand_helper<Protocol, Handler, has_post_strand<Protocol>::value>
    {};

    template <typename Protocol, typename Handler>
    typename post_strand_c<Protocol, Handler>::result_type post_strand(Protocol & proto, BOOST_ASIO_MOVE_ARG(Handler) handler)
    {
        return post_strand_c<Protocol, Handler>()(proto, BOOST_ASIO_MOVE_CAST(Handler)(handler));
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BEXIO_FWD_HPP__