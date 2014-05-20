#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_TEMPLATE_TYPE_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_TEMPLATE_TYPE_HPP__

#include <Bex/type_traits/defines.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有单个类型参数指定名字的成员模板函数或静态成员模板函数

/*
    class X
    {
        template <typename T>
        struct declare_struct;
    };
*/

namespace Bex
{

#define BEX_TT_HAS_TEMPLATE_TYPE(traits_name, template_name)                    \
    template <typename T>                                                       \
    struct traits_name                                                          \
    {                                                                           \
        struct decl_type;                                                       \
                                                                                \
        template <typename U>                                                   \
        static yes_type test(typename U::template template_name<decl_type> *);  \
                                                                                \
        template <typename U>                                                   \
        static no_type test(...);                                               \
                                                                                \
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type));     \
    };

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_TEMPLATE_TYPE_HPP__