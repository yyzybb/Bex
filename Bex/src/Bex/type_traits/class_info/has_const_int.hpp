#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_CONST_INT_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_CONST_INT_HPP__

#include <Bex/type_traits/defines.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否有指定名字的常量整型数

/*
    class X
    {
        enum { enum_value, };
        static const int sci_ = 0;
    };
*/

namespace Bex
{

#if !defined(BEX_SUPPORT_CXX11)

# define BEX_TT_HAS_CONSTEXPR(has_constexpr, constexpr_name)                \
    template <typename T>                                                   \
    struct has_constexpr                                                    \
    {                                                                       \
        template <int>                                                      \
        struct impl;                                                        \
                                                                            \
        template <typename U>                                               \
        static yes_type test( impl<U::constexpr_name>* );                   \
                                                                            \
        template <typename U>                                               \
        static no_type test(...);                                           \
                                                                            \
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type)); \
    };

#else  //!defined(BEX_SUPPORT_CXX11)

# define BEX_TT_HAS_ENUM(has_enum, enum_name)                                       \
    template <typename T>                                                           \
    struct has_enum                                                                 \
    {                                                                               \
        template <typename U>                                                       \
        static no_type test(...);                                                   \
                                                                                    \
        template <typename U>                                                       \
        static typename std::enable_if<                                             \
            std::is_enum<decltype(U::enum_name)>::value, yes_type>::type            \
        test(int*);                                                                 \
                                                                                    \
        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));   \
    };

# define BEX_TT_HAS_CONSTEXPR_INTEGER(has_constexpr_integer, constexpr_integer_name)    \
    template <typename T>                                                               \
    struct has_constexpr_integer                                                        \
    {                                                                                   \
        template <typename U>                                                           \
        static no_type test(...);                                                       \
                                                                                        \
        template <typename U, int C = U::constexpr_integer_name>                        \
        static typename std::enable_if<                                                 \
            !std::is_enum<decltype(U::constexpr_integer_name)>::value, yes_type>::type  \
            test(int*);                                                                 \
                                                                                        \
        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));       \
    };

# define BEX_TT_HAS_CONSTEXPR(has_constexpr, constexpr_name)                            \
    BEX_TT_HAS_ENUM(_##has_constexpr##_enum, constexpr_name)                            \
    BEX_TT_HAS_CONSTEXPR_INTEGER(_##has_constexpr##_constexpr_integer, constexpr_name)  \
    template <typename T>                                                               \
    struct has_constexpr                                                                \
    {                                                                                   \
        static const bool value = (_##has_constexpr##_enum<T>::value                    \
            || _##has_constexpr##_constexpr_integer<T>::value);                         \
    };


#endif //!defined(BEX_SUPPORT_CXX11)

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_CONST_INT_HPP__