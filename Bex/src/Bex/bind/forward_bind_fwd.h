#ifndef __BEX_BIND_FORWARD_BIND_FWD_H__
#define __BEX_BIND_FORWARD_BIND_FWD_H__

#include <tuple>
#include <type_traits>

namespace Bex
{
    namespace forward_bind
    {
        //////////////////////////////////////////////////////////////////////////
        /// 声明
        template <typename ... Args>
        using Tuple = std::tuple<Args...>;

        // 占位符
        template <int I>
        struct placeholder;

        // 可绑定callable对象的类型枚举
        enum class callable_type : int
        {
            free_function,      // 自由函数
            mem_function,       // 类成员函数
            object_function,    // 仿函数对象
            mem_variable,       // 类成员变量
        };

        // 调用方式(用于重载的类型定义)
        template <callable_type Ct>
        using call_t = std::integral_constant<callable_type, Ct>;

        using free_function_calltype = call_t<callable_type::free_function>;
        using mem_function_calltype = call_t<callable_type::mem_function>;
        using object_function_calltype = call_t<callable_type::object_function>;
        using mem_variable_calltype = call_t<callable_type::mem_variable>;

        // bind结果类
        template <typename R, typename F, typename ... BArgs>
        class bind_t;

        // callable对象类型萃取
        // 提供: 1.callable_type枚举值
        //      2.callable_type调用方式类型
        //      3.result_type
        //      4.实参个数
        template <typename F>
        struct callable_traits;

        // 占位符类型萃取
        // 提供: 1.是否是占位符的判断
        //      2.占位符索引号
        template <typename T>
        struct placeholder_traits;

        // 选择实参
        template <typename BindArgs, typename CallArgs, unsigned int N>
        struct select;

        // 整数常量转换为参数包个数的形式
        template <unsigned int ... N> struct seq {};
        template <unsigned int N, unsigned int ... S> struct gen;
        template <unsigned int N, unsigned int ... S> struct gen : gen<N - 1, N - 1, S...> {};
        template <unsigned int ...S> struct gen<0, S...>
        {
            typedef seq<S...> type;
        };

    } //namespace forward_bind
} //namespace Bex

#endif //__BEX_BIND_FORWARD_BIND_FWD_H__