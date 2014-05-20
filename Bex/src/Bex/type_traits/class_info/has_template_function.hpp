#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_TEMPLATE_FUNCTION_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_TEMPLATE_FUNCTION_HPP__

#include <Bex/type_traits/defines.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有单个类型参数指定名字的成员模板函数或静态成员模板函数

/*
    class X
    {
        template <typename T>
        void template_function();

        template <typename T>
        static void static_template_function();
    };
*/

namespace Bex
{
#if !defined(BEX_SUPPORT_CXX11)

#define BEX_TT_HAS_TEMPLATE_FUNCTION(traits_name, function_name)            \
    template <typename T>                                                   \
    struct traits_name                                                      \
    {                                                                       \
        struct decl_type;                                                   \
        template <typename U, bool>                                         \
        struct impl;                                                        \
                                                                            \
        template <typename U>                                               \
        static yes_type test(impl<U                                         \
            , &(U::template function_name<decl_type>) && 0>*);              \
                                                                            \
        template <typename U>                                               \
        static no_type test(...);                                           \
                                                                            \
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type)); \
    };

#else //!BEX_SUPPORT_CXX11

//#define BEX_TT_HAS_TEMPLATE_FUNCTION(traits_name, function_name)            \
//    template <typename T>                                                   \
//    struct traits_name                                                      \
//    {                                                                       \
//        struct decl_type;                                                   \
//        template <typename U, bool>                                         \
//        struct impl;                                                        \
//                                                                            \
//        template <typename U>                                               \
//        static yes_type test(impl<U                                         \
//            , &(U::template function_name<decl_type>) && 0>*);              \
//                                                                            \
//        template <typename U>                                               \
//        static no_type test(...);                                           \
//                                                                            \
//        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type)); \
//    };

#endif //!BEX_SUPPORT_CXX11

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_TEMPLATE_FUNCTION_HPP__