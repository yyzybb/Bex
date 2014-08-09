#ifndef __BEX_STREAM_SERIALIZATION_CONCEPT_HPP__
#define __BEX_STREAM_SERIALIZATION_CONCEPT_HPP__

#include <Bex/config.hpp>
#include <Bex/stream/serialization/serialization_fwd.h>
#include <Bex/type_traits/type_traits.hpp>
#include <boost/type_traits.hpp>

namespace Bex { namespace serialization
{
    /// 类型是否有版本号判断
    BEX_TT_HAS_CONSTEXPR(has_version_base, BEX_STREAM_SERIALIZATION_VERSION_NAME);
    template <typename T>
    struct has_version : has_version_base<typename remove_all<T>::type> {};

    /// 是否定义为模糊的读写类型
    //BEX_TT_HAS_TYPE(has_serialize_unkown_type, serialize_unkown_type);

    //////////////////////////////////////////////////////////////////////////
    /// @{ 类型对应的版本号
    template <typename T, bool _has_version>
    struct get_version_helper
    {
        static const int value = 0;
    };

    template <typename T>
    struct get_version_helper<T, true>
    {
        static const int value = T::BEX_STREAM_SERIALIZATION_VERSION_NAME;
    };

    template <typename T>
    struct get_version_base
        : get_version_helper<T, has_version<T>::value>
    {};

    template <typename T>
    struct get_version
        : get_version_base<typename remove_all<T>::type>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 判断类型是否已特化为配接器
    template <typename T>
    struct is_adapter_type_base
        : boost::false_type
    {};
    template <typename T>
    struct is_adapter_type_base<T*>
        : boost::true_type
    {};
    template <typename T, int N>
    struct is_adapter_type_base<T[N]>
        : boost::true_type
    {};
    template <typename T, typename Alloc>
    struct is_adapter_type_base<std::vector<T, Alloc> >
        : boost::true_type
    {};
    template <typename T, typename Alloc>
    struct is_adapter_type_base<std::list<T, Alloc> >
        : boost::true_type
    {};
    template <typename T, typename Traits, typename Alloc>
    struct is_adapter_type_base<std::basic_string<T, Traits, Alloc> >
        : boost::true_type
    {};
    template <typename K, typename T, typename Pr, typename Alloc>
    struct is_adapter_type_base<std::map<K, T, Pr, Alloc> >
        : boost::true_type
    {};
    template <typename T1, typename T2>
    struct is_adapter_type_base<std::pair<T1, T2> >
        : boost::true_type
    {};
    template <>
    struct is_adapter_type_base<binary_wrapper>
        : boost::true_type
    {};
    template <typename T>
    struct is_adapter_type_base<text_wrapper<T> >
        : boost::true_type
    {};
    template <typename T>
    struct is_adapter_type
        : is_adapter_type_base<typename boost::remove_cv<T>::type>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

#if !defined(BEX_SUPPORT_CXX11)
    BEX_TT_HAS_TEMPLATE_FUNCTION(has_serialize, serialize);
#else  //!defined(BEX_SUPPORT_CXX11)
    template <class C, bool>
    struct has_serialize_helper
    {
        struct Ar {};
        static Ar sar;

        template <class U>
        static no_type test(...);

        template <class U>
        static yes_type test(U*, decltype(make<U>().template serialize<Ar>(sar, unsigned()))* = nullptr);

        static const bool value = sizeof(test<C>(nullptr)) == sizeof(yes_type);
    };

    template <class C>
    struct has_serialize_helper<C, false> : std::false_type
    {};

    template <class C>
    struct has_serialize_base
        : has_serialize_helper<C, std::is_class<C>::value || std::is_union<C>::value>
    {};

    template <class C>
    struct has_serialize
        : has_serialize_base<C>
    {};
#endif //!defined(BEX_SUPPORT_CXX11)


#if !defined(BOOST_NO_CXX11_DECLTYPE)
    template <typename T>
    struct has_free_serialize_helper
    {
        struct Ar {};

        static const bool value = !std::is_same<
            decltype(serialize(std::declval<Ar&>(), std::declval<T&>(), (unsigned int)0)),
            decltype(serialize(std::declval<Ar&>(), std::declval<Ar&>(), (unsigned int)0))
        >::value;
    };

    template <>
    struct has_free_serialize_helper<void>
        : std::false_type
    {};

    template <typename T>
    struct has_free_serialize
        : has_free_serialize_helper<typename remove_all<T>::type>
    {};

    //////////////////////////////////////////////////////////////////////////
    /// @{ concept组合
    /// binary平凡类型判断(直接处理二进制流)
    template <typename T>
    struct is_binary_trivial
    {
        static const bool value = !boost::type_traits::ice_or<
            is_adapter_type<T>::value,
            has_serialize<T>::value,
            has_version<T>::value,
            has_free_serialize<T>::value
        >::value;
    };
    /// @}
    //////////////////////////////////////////////////////////////////////////
#else  //!defined(BOOST_NO_CXX11_DECLTYPE)
    template <typename T>
    struct is_binary_trivial
        : boost::is_arithmetic<T>
    {};
#endif //!defined(BOOST_NO_CXX11_DECLTYPE)

    //////////////////////////////////////////////////////////////////////////
    /// @{ 判断是否可批量处理优化
    template <typename T, typename ModeTag>
    struct is_optimize_helper;

    template <typename T>
    struct is_optimize_helper<T, binary_mode_tag>
        : is_binary_trivial<T>
    {};

    template <typename T>
    struct is_optimize_helper<T, text_mode_tag>
        : boost::is_same<T, char>
    {};

    template <typename T, class Ar>
    struct is_optimize
        : is_optimize_helper<T, typename archive_traits<Ar>::mode_tag>
    {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

} //namespace
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_CONCEPT_HPP__