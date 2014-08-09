#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_STATIC_MEMBER_DATA_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_STATIC_MEMBER_DATA_HPP__

#include <Bex/type_traits/defines.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有指定类型的静态成员变量

namespace Bex
{

#define BEX_TT_HAS_STATIC_MEMBER_DATA(traits_name, static_member_data_name)         \
    template <typename T, typename I>                                               \
    struct traits_name                                                              \
    {                                                                               \
        template <typename I, I*>                                                   \
        struct impl;                                                                \
                                                                                    \
        template <typename U, typename I>                                           \
        static yes_type test( impl<I, (&U::member_data_name)>* );                   \
                                                                                    \
        template <typename U, typename I>                                           \
        static no_type test(...);                                                   \
                                                                                    \
        static const bool value = (sizeof(test<T, I>(0)) == sizeof(yes_type));      \
    };

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_STATIC_MEMBER_DATA_HPP__