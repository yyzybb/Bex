#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_STATIC_MEMBER_FUNCTION_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_STATIC_MEMBER_FUNCTION_HPP__

#include <Bex/type_traits/defines.hpp>
#include <Bex/type_traits/signature_traits.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有指定函数签名的静态成员函数

namespace Bex
{

#define BEX_TT_HAS_STATIC_MEMBER_FUNCTION(traits_name, static_function_name)            \
    template <typename T, typename Signature>                                           \
    struct traits_name                                                                  \
    {                                                                                   \
        template <typename U, typename I                                                \
            , typename Bex::signature_traits<I>::function >                             \
        struct impl;                                                                    \
                                                                                        \
        template <typename U, typename I>                                               \
        static yes_type test( impl<U, I, (&U::static_function_name)>* );                \
                                                                                        \
        template <typename U, typename I>                                               \
        static no_type test(...);                                                       \
                                                                                        \
        static const bool value = (sizeof(test<T, Signature>(0)) == sizeof(yes_type));  \
    };

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_STATIC_MEMBER_FUNCTION_HPP__