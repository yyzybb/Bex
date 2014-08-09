#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_MEMBER_FUNCTION_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_MEMBER_FUNCTION_HPP__

#include <Bex/type_traits/defines.hpp>
#include <Bex/type_traits/signature_traits.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有指定函数签名的普通成员函数

namespace Bex
{

#define BEX_TT_HAS_MEMBER_FUNCTION(traits_name, function_name)                          \
    template <typename T, typename Signature>                                           \
    struct traits_name                                                                  \
    {                                                                                   \
        template <typename U, typename I                                                \
            , typename signature_traits<I>::member_function<U>::type >                  \
        struct impl;                                                                    \
                                                                                        \
        template <typename U, typename I>                                               \
        static yes_type test( impl<U, I, (&U::function_name)>* );                       \
                                                                                        \
        template <typename U, typename I>                                               \
        static no_type test(...);                                                       \
                                                                                        \
        static const bool value = (sizeof(test<T, Signature>(0)) == sizeof(yes_type));  \
    };

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_MEMBER_FUNCTION_HPP__