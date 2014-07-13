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
    struct has_initialize_2
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
    struct has_initialize_1
    {
        typedef typename next_layer_t<Protocol>::type NextProtocol;
        static const bool value = has_initialize_2<Protocol, F, Id>::value 
            || has_initialize_1<NextProtocol, F, Id
                , boost::is_same<typename next_layer_t<NextProtocol>::type, NextProtocol>::value>::value;
    };

    template <typename Protocol, typename F, typename Id>
    struct has_initialize_1<Protocol, F, Id, true>
        : has_initialize_2<Protocol, F, Id>
    {};

    /// 逐层检测
    template <typename Protocol, typename F, typename Id>
    struct has_initialize
        : has_initialize_1<Protocol, F, Id
            , boost::is_same<typename next_layer_t<Protocol>::type, Protocol>::value>
    {
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
    /// @{ invoker
    template <typename Protocol>
    struct has_invoker
    {
        template <typename U>
        static char _check(U*, typename U::invoker * = 0);

        template <typename U>
        static short _check(...);

        static const bool value = sizeof(_check<Protocol>((Protocol*)0)) == sizeof(char);
    };
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ async_handshake
    template <typename Protocol, typename Handler, bool>
    struct has_async_handshake_helper
    {
        typedef typename Protocol::invoker invoker;

        template <typename P, typename U, 
            void(U::*)(typename P::socket_ptr, ssl::stream_base::handshake_type, BEX_MOVE_ARG(Handler))>
        struct impl;

        template <typename P, typename U>
        static char _check(U*, impl<P, U, (&U::template async_handshake<Handler>) >* = 0);

        template <typename P, typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol, invoker>(0)) == sizeof(char));
    };

    template <typename Protocol, typename Handler>
    struct has_async_handshake_helper<Protocol, Handler, false>
        : public boost::false_type
    {};

    template <typename Protocol, typename Handler>
    struct has_async_handshake
        : has_async_handshake_helper<Protocol, Handler, has_invoker<Protocol>::value>
    {};

    template <typename Protocol, typename Handler, bool>
    struct async_handshake_helper
    {
        inline void operator()(typename Protocol::socket_ptr sp
            , typename Protocol::handshake_type hstype, BEX_MOVE_ARG(Handler) handler)
        {
            return typename Protocol::invoker().async_handshake(sp, hstype, BEX_MOVE_CAST(Handler)(handler));
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
    /// @{ handshake
    template <typename Protocol, bool>
    struct has_handshake_helper
    {
        typedef typename Protocol::invoker invoker;

        template <typename P, typename U, 
            void(U::*)(typename P::socket_ptr, ssl::stream_base::handshake_type, error_code&)>
        struct impl;

        template <typename P, typename U>
        static char _check(U*, impl<P, U, (&U::handshake) >* = 0);

        template <typename P, typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol, invoker>(0)) == sizeof(char));
    };

    template <typename Protocol>
    struct has_handshake_helper<Protocol, false>
        : public boost::false_type
    {};

    template <typename Protocol>
    struct has_handshake
        : has_handshake_helper<Protocol, has_invoker<Protocol>::value>
    {};

    template <typename Protocol, bool>
    struct handshake_helper
    {
        inline void operator()(typename Protocol::socket_ptr sp
            , typename Protocol::handshake_type hstype, error_code& ec)
        {
            return typename Protocol::invoker().handshake(sp, hstype, ec);
        }
    };

    template <typename Protocol>
    struct handshake_helper <Protocol, false>
    {
        inline void operator()(typename Protocol::socket_ptr
            , ssl::stream_base::handshake_type, error_code& ec)
        {
            ec.clear();
        }
    };

    template <typename Protocol>
    struct handshake_c
        : handshake_helper<Protocol, has_handshake<Protocol>::value>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ async_shutdown
    template <typename Protocol, typename Handler, bool>
    struct has_async_shutdown_helper
    {
        typedef typename Protocol::invoker invoker;

        template <typename P, typename U, 
            void(U::*)(typename P::socket_ptr, BEX_MOVE_ARG(Handler))>
        struct impl;

        template <typename P, typename U>
        static char _check(U*, impl<P, U, (&U::template async_handshake<Handler>) >* = 0);

        template <typename P, typename U>
        static short _check(...);

        static const bool value = (sizeof(_check<Protocol, invoker>(0)) == sizeof(char));
    };

    template <typename Protocol, typename Handler>
    struct has_async_shutdown_helper<Protocol, Handler, false>
        : boost::false_type
    {};

    template <typename Protocol, typename Handler>
    struct has_async_shutdown
        : has_async_shutdown_helper<Protocol, Handler, has_invoker<Protocol>::value>
    {};

    template <typename Protocol, typename Handler, bool>
    struct async_shutdown_helper
    {
        inline void operator()(typename Protocol::socket_ptr sp, BEX_MOVE_ARG(Handler) handler)
        {
            return typename Protocol::invoker().async_shutdown(sp, BEX_MOVE_CAST(Handler)(handler));
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

        // 异步握手
        template <typename Handler>
        static void async_handshake(socket_ptr sp,
            ssl::stream_base::handshake_type hstype, BEX_MOVE_ARG(Handler) handler)
        {
            async_handshake_c<Protocol, Handler>()(sp, hstype, BEX_MOVE_CAST(Handler)(handler));
        }

        // 同步握手
        static void handshake(socket_ptr sp, ssl::stream_base::handshake_type hstype, error_code & ec)
        {
            handshake_c<Protocol>()(sp, hstype, ec);
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