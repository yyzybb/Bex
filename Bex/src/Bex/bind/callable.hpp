#ifndef __BEX_BIND_CALLABLE_HPP__
#define __BEX_BIND_CALLABLE_HPP__

#include <Bex/bind/forward_bind_fwd.h>

namespace Bex { namespace forward_bind
{
    // free function
    template <typename R, typename ... CArgs>
    struct callable_traits<R(*)(CArgs...)>
    {
        static const callable_type ct = callable_type::free_function;
        static const int parameter_count = sizeof...(CArgs);

        typedef call_t<ct> call_type;
        typedef R result_type;
        typedef int class_type;
    };

    // mem_function
    template <class C, typename R, typename ... CArgs>
    struct callable_traits<R(C::*)(CArgs...)>
    {
        static const callable_type ct = callable_type::mem_function;
        static const int parameter_count = sizeof...(CArgs)+1;

        typedef call_t<ct> call_type;
        typedef R result_type;
        typedef C class_type;
    };

    template <class C, typename R, typename ... CArgs>
    struct callable_traits<R(C::*)(CArgs...) const>
        : callable_traits<R(C::*)(CArgs...)>
    {};
    template <class C, typename R, typename ... CArgs>
    struct callable_traits<R(C::*)(CArgs...) volatile>
        : callable_traits<R(C::*)(CArgs...)>
    {};
    template <class C, typename R, typename ... CArgs>
    struct callable_traits<R(C::*)(CArgs...) const volatile>
        : callable_traits<R(C::*)(CArgs...)>
    {};

    // object_function
    template <class Callable>
    struct callable_traits
    {
        typedef callable_traits<decltype(&Callable::operator())> base_type;

        static const callable_type ct = callable_type::object_function;
        static const int parameter_count = base_type::parameter_count - 1;

        typedef call_t<ct> call_type;
        typedef typename base_type::result_type result_type;
        typedef Callable class_type;
    };

    // mem_variable
    template <typename C, typename R>
    struct callable_traits<R(C::*)>
    {
        static const callable_type ct = callable_type::mem_variable;
        static const int parameter_count = 1;

        typedef call_t<ct> call_type;
        typedef R result_type;
        typedef C class_type;
    };

} //namespace forward_bind
} //namespace Bex

#endif //__BEX_BIND_CALLABLE_HPP__