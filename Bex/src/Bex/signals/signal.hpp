#ifndef __BEX_SIGNALS_SIGNAL__
#define __BEX_SIGNALS_SIGNAL__

#include "signal_fwd.h"
#include "combine_last_value.hpp"
#include "combine_accumulate_value.hpp"

//////////////////////////////////////////////////////////////////////////
/// 基于signals2的观察者模式框架

namespace Bex
{
    template <typename Signature>
    struct sign_wrapper
    {
        typedef Signature type;
    };

    template <int Group, int Index>
    struct signal_traits;

    template <int Group, int Index>
    struct SignalHolder
    {
        typedef signal_traits<Group, Index> traits;
        typedef boost_signals2::signal<
            typename traits::Signature,
            typename traits::Combiner> signal_type;

        static signal_type & get_signal()
        {
            static signal_type obj;
            return obj;
        }
    };

    namespace
    {
        /// 取最后一个返回值的合并器
        using detail::combine_last_value;

        /// 累加所有返回值的合并器
        using detail::combine_accumulate_value;
    }
}

/// 获取指定(组, 编号)的signal.
// @Group 组索引, 必须是编译器常量, 如 const int, enum等.
// @Index 组中的编号索引, 必须是编译器常量, 如 const int, enum等.
#define BEX_GET_SIGNAL(Group, Index) (::Bex::SignalHolder<Group, Index>::get_signal())

/// 定义signal
// @Signature 回调函数类型, 如: void(), int(double, void) 等.
#define BEX_DEFINE_SIGNAL(Group, Index, signature)                              \
namespace Bex {                                                                 \
    template <>                                                                 \
    struct signal_traits<Group, Index>                                          \
    {                                                                           \
        typedef sign_wrapper<signature>::type Signature;                        \
        typedef combine_last_value<typename                                     \
             boost::function_traits<signature>::result_type> Combiner;          \
    };                                                                          \
}

/// 定义使用指定合并器的signal
#define BEX_DEFINE_COMBINE_SIGNAL(Group, Index, signature, combiner)            \
namespace Bex {                                                                 \
    template <>                                                                 \
    struct signal_traits<Group, Index>                                          \
    {                                                                           \
        typedef sign_wrapper<signature>::type Signature;                        \
        typedef combiner<typename                                               \
            boost::function_traits<signature>::result_type> Combiner;           \
    };                                                                          \
}


#endif //__BEX_SIGNALS_SIGNAL__