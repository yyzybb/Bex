#ifndef __BEX_BIND_SELECT_HPP__
#define __BEX_BIND_SELECT_HPP__

#include <Bex/bind/forward_bind_fwd.h>

namespace Bex { namespace forward_bind
{
    template <typename BindArgs, typename CallArgs, int N, bool>
    struct select_helper
    {
        using result_type = typename std::tuple_element<N, BindArgs>::type&;

        inline static result_type invoke(BindArgs & bargs, CallArgs &)
        {
            return std::get<N>(bargs);
        }
    };

    template <typename BindArgs, typename CallArgs, int N>
    struct select_helper<BindArgs, CallArgs, N, true>
    {
        using bind_arg_type = typename std::tuple_element<N, BindArgs>::type;
        using ph_traits = placeholder_traits<typename std::decay<bind_arg_type>::type>;
        static const int I = ph_traits::from_zero_index;
        using result_type = typename std::tuple_element<I, CallArgs>::type;

        static_assert(ph_traits::is_placeholder, "Fatal Error!");

        inline static result_type invoke(BindArgs & bargs, CallArgs & cargs)
        {
            return (result_type)std::get<I>(cargs);
        }
    };

    template <unsigned int N, typename ... BArgs, typename ... CArgs>
    struct select<Tuple<BArgs...>, Tuple<CArgs...>, N>
    {
        typedef Tuple<BArgs...> BindArgs;
        typedef Tuple<CArgs...> CallArgs;

        using bind_arg_type = typename std::tuple_element<N, BindArgs>::type;
        using ph_traits = placeholder_traits<typename std::decay<bind_arg_type>::type>;
        static const bool is_placeholder = ph_traits::is_placeholder;

        typedef select_helper<BindArgs, CallArgs, N, is_placeholder> helper;

        inline static typename helper::result_type invoke(BindArgs & bargs, CallArgs & cargs)
        {
            return helper::invoke(bargs, cargs);
        }
    };

} //namespace forward_bind
} //namespace Bex

#endif //__BEX_BIND_SELECT_HPP__