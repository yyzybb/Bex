#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_TYPE_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_TYPE_HPP__

#include <Bex/type_traits/defines.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有指定类型

namespace Bex
{

#define BEX_TT_HAS_TYPE(traits_name, _type_name)                            \
    template <typename T>                                                   \
    struct traits_name                                                      \
    {                                                                       \
        template <typename U>                                               \
        static yes_type test(typename U::_type_name*);                      \
                                                                            \
        template <typename U>                                               \
        static no_type test(...);                                           \
                                                                            \
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type)); \
    };

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_TYPE_HPP__