#ifndef __BEX_IO_PROTOCOL_TRAITS_HPP__
#define __BEX_IO_PROTOCOL_TRAITS_HPP__

//////////////////////////////////////////////////////////////////////////
/// 协议相关接口处理
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio 
{
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

    template <typename Addition, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    struct function_addition<Addition, boost::function<R(Arg1, Arg2, Arg3, Arg4)> >
        : boost::mpl::identity<boost::function<R(Addition, Arg1, Arg2, Arg3, Arg4)> >
    {};

    //////////////////////////////////////////////////////////////////////////
    /// @{ post strand
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
        result_type operator()(Protocol & proto, Handler const& handler)
        {
            return proto.post_strand(handler);
        }
    };

    template <typename Protocol, typename Handler>
    struct post_strand_helper<Protocol, Handler, false>
    {
        typedef Handler const& result_type;
        result_type operator()(Protocol &, Handler const& handler)
        {
            return handler;
        }
    };

    template <typename Protocol, typename Handler>
    struct post_strand_c
        : post_strand_helper<Protocol, Handler, has_post_strand<Protocol>::value>
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

    template <typename Protocol>
    struct protocol_traits
    {
        // 初始化
        template <typename F, typename Id>
        static void initialize(Protocol & proto, shared_ptr<options> const& opts, F const& f, Id const& id)
        {
            initialize_c<Protocol, F, Id>()(proto, opts, f, id);
        }

        // 包装回调函数
        template <typename Handler>
        static typename post_strand_c<Protocol, Handler>::result_type post_strand(Protocol & proto, Handler const& handler)
        {
            return post_strand_c<Protocol, Handler>()(proto, handler);
        }
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_PROTOCOL_TRAITS_HPP__