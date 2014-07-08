#ifndef __BEX_IO_PROTOCOL_TRAITS_HPP__
#define __BEX_IO_PROTOCOL_TRAITS_HPP__

//////////////////////////////////////////////////////////////////////////
/// 协议相关接口处理
#include "bexio_fwd.hpp"
#include <boost/asio/ssl/stream_base.hpp>

namespace Bex { namespace bexio 
{
    //////////////////////////////////////////////////////////////////////////
    /// @{ boost::function addition
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

    template <typename Addition, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    struct function_addition<Addition, boost::function<R(Arg1, Arg2, Arg3, Arg4)> >
        : boost::mpl::identity<boost::function<R(Addition, Arg1, Arg2, Arg3, Arg4)> >
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ initialize
    template <typename Protocol, typename F, typename Id>
    struct has_initialize
    {
        template <typename U, void(U::*)(shared_ptr<options> const&, F const&, Id const&)>
        struct impl;

        template <typename U>
        static char _check(U*, impl<U, (&U::template initialize<F, Id>) >*);

        template <typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol>(0, 0)) == sizeof(char));
    };

    template <typename Protocol, typename F, typename Id, bool>
    struct initialize_helper
    {
        inline void operator()(Protocol & proto, shared_ptr<options> const& opts, F const& f, Id const& id)
        {
            return proto.initialize(opts, f, id);
        }
    };

    template <typename Protocol, typename F, typename Id>
    struct initialize_helper<Protocol, F, Id, false>
    {
        inline void operator()(Protocol &, shared_ptr<options> const&, F const&, Id const&)
        { }
    };

    template <typename Protocol, typename F, typename Id>
    struct initialize_c
        : initialize_helper<Protocol, F, Id, has_initialize<Protocol, F, Id>::value>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ async_handshake
    template <typename Protocol, typename Handler>
    struct has_async_handshake
    {
        template <typename U, void(*)(typename U::socket_ptr, ssl::stream_base::handshake_type, BEX_MOVE_ARG(Handler))>
        struct impl;

        template <typename U>
        static char _check(U*, impl<U, (&U::template async_handshake<Handler>) >*);

        template <typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol>(0, 0)) == sizeof(char));
    };

    template <typename Protocol, typename Handler, bool>
    struct async_handshake_helper
    {
        inline void operator()(typename Protocol::socket_ptr sp
            , typename Protocol::handshake_type hstype, BEX_MOVE_ARG(Handler) handler)
        {
            return Protocol::async_handshake(sp, hstype, BEX_MOVE_CAST(Handler)(handler));
        }
    };

    template <typename Protocol, typename Handler>
    struct async_handshake_helper <Protocol, Handler, false>
    {
        inline void operator()(typename Protocol::socket_ptr
            , ssl::stream_base::handshake_type, BEX_MOVE_ARG(Handler) handler)
        {
            handler(error_code());
        }
    };

    template <typename Protocol, typename Handler>
    struct async_handshake_c
        : async_handshake_helper<Protocol, Handler, has_async_handshake<Protocol, Handler>::value>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ async_shutdown
    template <typename Protocol, typename Handler>
    struct has_async_shutdown
    {
        template <typename U, void(*)(typename U::socket_ptr, BEX_MOVE_ARG(Handler))>
        struct impl;

        template <typename U>
        static char _check(U*, impl<U, (&U::template async_shutdown<Handler>) >*);

        template <typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol>(0, 0)) == sizeof(char));
    };

    template <typename Protocol, typename Handler, bool>
    struct async_shutdown_helper
    {
        inline void operator()(typename Protocol::socket_ptr sp, BEX_MOVE_ARG(Handler) handler)
        {
            return Protocol::async_shutdown(sp, BEX_MOVE_CAST(Handler)(handler));
        }
    };

    template <typename Protocol, typename Handler>
    struct async_shutdown_helper <Protocol, Handler, false>
    {
        inline void operator()(typename Protocol::socket_ptr, BEX_MOVE_ARG(Handler) handler)
        {
            handler(error_code());
        }
    };

    template <typename Protocol, typename Handler>
    struct async_shutdown_c
        : async_shutdown_helper<Protocol, Handler, has_async_shutdown<Protocol, Handler>::value>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////
    template <typename Protocol>
    struct protocol_traits
    {
        typedef typename Protocol::socket_ptr socket_ptr;

        // 初始化
        template <typename F, typename Id>
        static void initialize(Protocol & proto, shared_ptr<options> const& opts, F const& f, Id const& id)
        {
            initialize_c<Protocol, F, Id>()(proto, opts, f, id);
        }

        // 握手
        template <typename Handler>
        static void async_handshake(socket_ptr sp,
            ssl::stream_base::handshake_type hstype, BEX_MOVE_ARG(Handler) handler)
        {
            async_handshake_c<Protocol, Handler>()(sp, hstype, BEX_MOVE_CAST(Handler)(handler));
        }

        // 异步优雅地关闭
        template <typename Handler>
        static void async_shutdown(socket_ptr sp, BEX_MOVE_ARG(Handler) handler)
        {
            async_shutdown_c<Protocol, Handler>()(sp, BEX_MOVE_CAST(Handler)(handler));
        }
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_PROTOCOL_TRAITS_HPP__