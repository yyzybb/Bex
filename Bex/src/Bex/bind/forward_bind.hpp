#ifndef __BEX_BIND_FORWARD_BIND_HPP__
#define __BEX_BIND_FORWARD_BIND_HPP__

#include <Bex/bind/forward_bind_fwd.h>
#include <Bex/bind/callable.hpp>
#include <Bex/bind/placeholder.hpp>
#include <Bex/bind/select.hpp>
#include <boost/preprocessor.hpp>

namespace Bex { namespace forward_bind
{
    //////////////////////////////////////////////////////////////////////////
    /// 返回类型非void
    template <typename R, typename F, typename ... BArgs>
    class bind_t
    {
        using f_traits = callable_traits<F>;
        using result_type = R;
        using call_type = typename f_traits::call_type;

        static_assert(f_traits::parameter_count == sizeof...(BArgs), "bind parameter count must be matched!");

        using BindArgs = Tuple<BArgs...>;

        F m_fun;
        BindArgs m_args;

    public:
        template <typename ... Args>
        explicit bind_t(F const& fun, Args &&... args)
            : m_fun(fun), m_args(std::forward<Args>(args)...) {}

    public:
        template <typename ... CArgs>
        result_type operator()(CArgs && ... cargs)
        {
            return call_fun(call_type(), typename gen<f_traits::parameter_count>::type(), std::forward<CArgs>(cargs)...);
        }

    private:
        //////////////////////////////////////////////////////////////////////////
        /// @{ call free function
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(free_function_calltype, seq<S...>, CArgs && ... cargs)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            return m_fun(select<BindArgs, CallArgs, S>::invoke(m_args, cargs_tuple)...);
        }
        /// @}
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        /// @{ call mem function
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(mem_function_calltype, seq<S...>, CArgs && ... cargs)
        {
            using seq_type = typename gen<sizeof...(S)-1>::type;
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            return call_mem_fun<CArgs...>(seq_type()
                , select<BindArgs, CallArgs, 0>::invoke(m_args, cargs_tuple)
                , cargs_tuple);
        }

        template <typename ... CArgs, unsigned int ... S>
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type * ptr, Tuple<CArgs&&...> & cargs_tuple)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            return (ptr->*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...);
        }
        template <typename ... CArgs, unsigned int ... S>
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type & ref, Tuple<CArgs&&...> & cargs_tuple)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            return (ref.*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...);
        }

#define BEX_FORWARD_BIND_CALL_MEM_FUN(cv) \
        template <typename ... CArgs, unsigned int ... S> \
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type cv* ptr, Tuple<CArgs&&...> & cargs_tuple) \
        { \
            typedef Tuple<CArgs&&...> CallArgs; \
            return (ptr->*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...); \
        } \
        template <typename ... CArgs, unsigned int ... S> \
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type cv& ref, Tuple<CArgs&&...> & cargs_tuple) \
        { \
            typedef Tuple<CArgs&&...> CallArgs; \
            return (ref.*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...); \
        }

        BEX_FORWARD_BIND_CALL_MEM_FUN(const);
        BEX_FORWARD_BIND_CALL_MEM_FUN(volatile);
        BEX_FORWARD_BIND_CALL_MEM_FUN(const volatile);
#undef BEX_FORWARD_BIND_CALL_MEM_FUN
        /// @}
        //////////////////////////////////////////////////////////////////////////    

        //////////////////////////////////////////////////////////////////////////
        /// @{ call object function
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(object_function_calltype, seq<S...>, CArgs && ... cargs)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            return m_fun(select<BindArgs, CallArgs, S>::invoke(m_args, cargs_tuple)...);
        }
        /// @}
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        /// @{ call mem variable
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(mem_variable_calltype, seq<S...>, CArgs && ... cargs)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            return call_mem_variable(select<BindArgs, CallArgs, 0>::invoke(m_args, cargs_tuple));
        }

        template <typename ... CArgs>
        result_type call_mem_variable(typename f_traits::class_type * ptr, CArgs && ...)
        {
            return (ptr->*m_fun);
        }
        template <typename ... CArgs>
        result_type call_mem_variable(typename f_traits::class_type & ref, CArgs && ...)
        {
            return (ref.*m_fun);
        }

#define BEX_FORWARD_BIND_CALL_MEM_VARIABLE(cv) \
        template <typename ... CArgs> \
        result_type call_mem_variable(typename f_traits::class_type cv* ptr, CArgs && ...) \
        { \
            return (ptr->*m_fun); \
        } \
        template <typename ... CArgs> \
        result_type call_mem_variable(typename f_traits::class_type cv& ref, CArgs && ...) \
        { \
            return (ref.*m_fun); \
        }

        BEX_FORWARD_BIND_CALL_MEM_VARIABLE(const);
        BEX_FORWARD_BIND_CALL_MEM_VARIABLE(volatile);
        BEX_FORWARD_BIND_CALL_MEM_VARIABLE(const volatile);
        /// @}
        //////////////////////////////////////////////////////////////////////////
    };

    //////////////////////////////////////////////////////////////////////////
    /// 返回类型void特化
    template <typename F, typename ... BArgs>
    class bind_t<void, F, BArgs...>
    {
        using f_traits = callable_traits<F>;
        using result_type = void;
        using call_type = typename f_traits::call_type;

        static_assert(f_traits::parameter_count == sizeof...(BArgs), "bind parameter count must be matched!");

        using BindArgs = Tuple<BArgs...>;

        F m_fun;
        BindArgs m_args;

    public:
        template <typename ... Args>
        explicit bind_t(F const& fun, Args &&... args)
            : m_fun(fun), m_args(std::forward<Args>(args)...) {}

    public:
        template <typename ... CArgs>
        result_type operator()(CArgs && ... cargs)
        {
            call_fun(call_type(), typename gen<f_traits::parameter_count>::type(), std::forward<CArgs>(cargs)...);
        }

    private:
        //////////////////////////////////////////////////////////////////////////
        /// @{ call free function
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(free_function_calltype, seq<S...>, CArgs && ... cargs)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            m_fun(select<BindArgs, CallArgs, S>::invoke(m_args, cargs_tuple)...);
        }
        /// @}
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        /// @{ call mem function
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(mem_function_calltype, seq<S...>, CArgs && ... cargs)
        {
            using seq_type = typename gen<sizeof...(S)-1>::type;
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            call_mem_fun<CArgs...>(seq_type()
                , select<BindArgs, CallArgs, 0>::invoke(m_args, cargs_tuple)
                , cargs_tuple);
        }

        template <typename ... CArgs, unsigned int ... S>
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type * ptr, Tuple<CArgs&&...> & cargs_tuple)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            (ptr->*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...);
        }
        template <typename ... CArgs, unsigned int ... S>
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type & ref, Tuple<CArgs&&...> & cargs_tuple)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            (ref.*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...);
        }

#define BEX_FORWARD_BIND_CALL_MEM_FUN(cv) \
        template <typename ... CArgs, unsigned int ... S> \
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type cv* ptr, Tuple<CArgs&&...> & cargs_tuple) \
        { \
            typedef Tuple<CArgs&&...> CallArgs; \
            (ptr->*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...); \
        } \
        template <typename ... CArgs, unsigned int ... S> \
        result_type call_mem_fun(seq<S...>, typename f_traits::class_type cv& ref, Tuple<CArgs&&...> & cargs_tuple) \
        { \
            typedef Tuple<CArgs&&...> CallArgs; \
            (ref.*m_fun)(select<BindArgs, CallArgs, S + 1>::invoke(m_args, cargs_tuple)...); \
        }

        BEX_FORWARD_BIND_CALL_MEM_FUN(const);
        BEX_FORWARD_BIND_CALL_MEM_FUN(volatile);
        BEX_FORWARD_BIND_CALL_MEM_FUN(const volatile);
#undef BEX_FORWARD_BIND_CALL_MEM_FUN
        /// @}
        //////////////////////////////////////////////////////////////////////////    

        //////////////////////////////////////////////////////////////////////////
        /// @{ call object function
        template <unsigned int ... S, typename ... CArgs>
        result_type call_fun(object_function_calltype, seq<S...>, CArgs && ... cargs)
        {
            typedef Tuple<CArgs&&...> CallArgs;
            CallArgs cargs_tuple((CArgs&&)cargs...);
            m_fun(select<BindArgs, CallArgs, S>::invoke(m_args, cargs_tuple)...);
        }
        /// @}
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        /// @{ call mem variable
        // It can't call mem variable when result_type is void.
        /// @}
        //////////////////////////////////////////////////////////////////////////
    };

#ifndef BEX_BIND
#define BEX_BIND bind
#endif 

        template <typename F, typename ... Args>
        bind_t<typename callable_traits<F>::result_type, F, typename std::decay<Args>::type...>
            BEX_BIND(F const& f, Args && ... args)
        {
            return bind_t<typename callable_traits<F>::result_type, F, typename std::decay<Args>::type...>
                (f, std::forward<Args>(args)...);
        }
    } //namespace forward_bind

    using forward_bind::BEX_BIND;

//////////////////////////////////////////////////////////////////////////
// args (_1, _2, _3, ..., _32)

#ifndef BEX_FORWARD_BIND_PARAMETER_COUNT
#define BEX_FORWARD_BIND_PARAMETER_COUNT 32
#endif

#define BOOST_PP_LOCAL_MACRO(n) \
    static forward_bind::placeholder<n> _##n;

#define BOOST_PP_LOCAL_LIMITS (1, BEX_FORWARD_BIND_PARAMETER_COUNT)
#include BOOST_PP_LOCAL_ITERATE()

} //namespace Bex

#endif //__BEX_BIND_FORWARD_BIND_HPP__