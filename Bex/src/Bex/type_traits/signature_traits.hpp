#ifndef __BEX_TYPE_TRAITS_SIGNATURE_TRAITS_HPP__
#define __BEX_TYPE_TRAITS_SIGNATURE_TRAITS_HPP__

#include <boost/preprocessor.hpp>
#include <boost/static_assert.hpp>
//////////////////////////////////////////////////////////////////////////
/// Signature Traits
/// 函数签名类型特征定义

namespace Bex
{
    template <typename Signature>
    struct signature_traits;

    //template <typename R>
    //struct signature_traits<R()>
    //{
    //    typedef R result_type;

    //    typedef R(*function)();

    //    template <typename C>
    //    struct member_function
    //    {
    //        typedef R(C::*type)();
    //    };
    //};

    //template <typename R, typename T1>
    //struct signature_traits<R(T1)>
    //{
    //    typedef R result_type;

    //    typedef R(*function)(T1);

    //    template <typename C>
    //    struct member_function
    //    {
    //        typedef R(C::*type)(T1);
    //    };
    //};

#define BOOST_PP_LOCAL_MACRO(n)                                             \
    template <typename R                                                    \
        BOOST_PP_COMMA_IF(n)                                                \
        BOOST_PP_ENUM_PARAMS(n, typename T)>                                \
    struct signature_traits<R(BOOST_PP_ENUM_PARAMS(n, T))>                  \
    {                                                                       \
        typedef R result_type;                                              \
                                                                            \
        typedef R(*function)(BOOST_PP_ENUM_PARAMS(n, T));                   \
                                                                            \
        template <typename C>                                               \
        struct member_function                                              \
        {                                                                   \
            typedef R(C::*type)(BOOST_PP_ENUM_PARAMS(n, T));                \
        };                                                                  \
    };
#define BOOST_PP_LOCAL_LIMITS (0, 9)
#include BOOST_PP_LOCAL_ITERATE()

#define BOOST_PP_LOCAL_MACRO(n)                                             \
    template <typename R                                                    \
        BOOST_PP_COMMA_IF(n)                                                \
        BOOST_PP_ENUM_PARAMS(n, typename T)>                                \
    struct signature_traits<R(*)(BOOST_PP_ENUM_PARAMS(n, T))>               \
    {                                                                       \
        typedef R result_type;                                              \
                                                                            \
        typedef R(*function)(BOOST_PP_ENUM_PARAMS(n, T));                   \
                                                                            \
        template <typename C>                                               \
        struct member_function                                              \
        {                                                                   \
            typedef R(C::*type)(BOOST_PP_ENUM_PARAMS(n, T));                \
        };                                                                  \
    };
#define BOOST_PP_LOCAL_LIMITS (0, 9)
#include BOOST_PP_LOCAL_ITERATE()

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_SIGNATURE_TRAITS_HPP__