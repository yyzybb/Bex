#ifndef __BEX_STREAM_SERIALIZATION_CONCEPT_HPP__
#define __BEX_STREAM_SERIALIZATION_CONCEPT_HPP__

#include "serialization_fwd.h"
#include "version_adl.hpp"

namespace Bex { namespace serialization
{
    //////////////////////////////////////////////////////////////////////////
    /// @{ 类型是否有版本号判断
    namespace detail
    {
        template <typename T>
        struct has_mem_version
        {
            template <typename U>
            static no_type check(...);

            template <typename U>
            static yes_type check(decltype(std::declval<U*>()->serialize_version())*);

            static const bool value = (sizeof(check<T>(nullptr)) == sizeof(yes_type));
        };

        template <typename T>
        class has_free_version_helper
        {
            struct internal_type {};

        public:
            static const bool value = !boost::is_same<
                decltype(serialize_version((T*)nullptr)),
                decltype(serialize_version((internal_type*)nullptr))
            >::value;
        };

        template <>
        class has_free_version_helper<void> : public boost::false_type {};

        template <typename T>
        struct has_free_version : has_free_version_helper<T> {};
    } //namespace detail

    template <typename T>
    struct has_version
        : boost::type_traits::ice_or<
            detail::has_mem_version<typename remove_all<T>::type>::value,
            detail::has_free_version<typename remove_all<T>::type>::value
        >
    {};

    // 类型对应的版本号
    template <typename T>
    typename boost::enable_if<has_version<T>, unsigned int>::type get_version()
    {
        return serialize_version_adl((typename remove_all<T>::type*)nullptr);
    }

    template <typename T>
    typename boost::disable_if<has_version<T>, unsigned int>::type get_version()
    {
        return 0;
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 判断类型是否已特化为配接器
    template <typename T>
    struct is_adapter_type;

    template <typename T>
    struct is_adapter_type_impl : boost::false_type {};

    /// ------- STL Containers
    template <typename T, typename Alloc>
    struct is_adapter_type_impl<std::vector<T, Alloc> > : boost::true_type {};

    template <typename T, typename Traits, typename Alloc>
    struct is_adapter_type_impl<std::basic_string<T, Traits, Alloc> > : boost::true_type {};

    template <typename T, typename Alloc>
    struct is_adapter_type_impl<std::list<T, Alloc> > : boost::true_type {};

    template <typename T, typename Alloc>
    struct is_adapter_type_impl<std::deque<T, Alloc> > : boost::true_type {};

    template <typename T, typename Pr, typename Alloc>
    struct is_adapter_type_impl<std::set<T, Pr, Alloc> > : boost::true_type {};

    template <typename T, typename Pr, typename Alloc>
    struct is_adapter_type_impl<std::multiset<T, Pr, Alloc> > : boost::true_type {};

    template <typename K, typename T, typename Pr, typename Alloc>
    struct is_adapter_type_impl<std::map<K, T, Pr, Alloc> > : boost::true_type {};

    template <typename K, typename T, typename Pr, typename Alloc>
    struct is_adapter_type_impl<std::multimap<K, T, Pr, Alloc> > : boost::true_type {};

#if defined(_MSC_VER)
    template <typename K, typename T, typename Pr, typename Alloc>
    struct is_adapter_type_impl<stdext::hash_map<K, T, Pr, Alloc> > : boost::true_type {};
#endif //defined(_MSC_VER)

    template <typename K, typename T, typename H, typename Eq, typename Alloc>
    struct is_adapter_type_impl<std::unordered_map<K, T, H, Eq, Alloc> > : boost::true_type {};

    template <typename K, typename T, typename H, typename Eq, typename Alloc>
    struct is_adapter_type_impl<std::unordered_multimap<K, T, H, Eq, Alloc> > : boost::true_type {};

    template <typename T, typename H, typename Eq, typename Alloc>
    struct is_adapter_type_impl<std::unordered_set<T, H, Eq, Alloc> > : boost::true_type {};

    template <typename T, typename H, typename Eq, typename Alloc>
    struct is_adapter_type_impl<std::unordered_multiset<T, H, Eq, Alloc> > : boost::true_type {};

    template <typename T1, typename T2>
    struct is_adapter_type_impl<std::pair<T1, T2> > : boost::true_type {};

    template <typename T, std::size_t N>
    struct is_adapter_type_impl<std::array<T, N> > : boost::true_type {};

    template <typename ... CArgs>
    struct is_adapter_type_impl<std::tuple<CArgs...> > : boost::true_type {};

    /// -------- Boost Containers
    template <typename T, std::size_t N>
    struct is_adapter_type_impl<boost::array<T, N> > : boost::true_type {};

    template <typename K1, typename K2, typename AP1, typename AP2, typename AP3>
    struct is_adapter_type_impl<boost::bimap<K1, K2, AP1, AP2, AP3> > : boost::true_type {};

    /// -------- Wrappers
    template <>
    struct is_adapter_type_impl<binary_wrapper> : boost::true_type {};

    template <typename T>
    struct is_adapter_type_impl<text_wrapper<T> > : boost::true_type {};

    /// -------- pointer and C-style array
    template <typename T>
    struct is_adapter_type<T*> : boost::true_type {};

    template <typename T, int N>
    struct is_adapter_type<T[N]> : boost::true_type {};

    template <typename T>
    struct is_adapter_type : is_adapter_type_impl<typename remove_all<T>::type> {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ optimize
    namespace detail
    {
        template <class T, bool>
        struct has_mem_serialize_helper
        {
            struct Ar {};

            template <class U>
            static no_type test(...);

            template <class U>
            static yes_type test(U*
                , decltype(std::declval<U>().template serialize<Ar>(std::declval<Ar&>(), unsigned()))* = nullptr);

            static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes_type);
        };

        template <class T>
        struct has_mem_serialize_helper<T, false> : boost::false_type {};

        template <class T>
        struct has_mem_serialize 
            : has_mem_serialize_helper<T, boost::is_class<T>::value || boost::is_union<T>::value>
        {};

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
        struct has_free_serialize_helper<void> : std::false_type {};

        template <typename T>
        struct has_free_serialize
            : has_free_serialize_helper<T>
        {};
    } //namespace detail

    template <typename T>
    struct has_serialize
        : boost::type_traits::ice_or<
            detail::has_mem_serialize<typename remove_all<T>::type>::value,
            detail::has_free_serialize<typename remove_all<T>::type>::value
        >
    {};

    // binary平凡类型判断(直接处理二进制流)
    template <typename T>
    struct is_binary_trivial
    {
        static const bool value = !boost::type_traits::ice_or<
            is_adapter_type<T>::value,
            has_serialize<T>::value,
            has_version<T>::value
        >::value 
#if !defined(BEX_SERIALIZATION_USE_POD_EXTEND)
        && std::is_pod<typename remove_all<T>::type>::value
#endif //!defined(BEX_SERIALIZATION_USE_POD_EXTEND)
        ;
    };

    // 判断是否可批量处理优化
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

} //namespace serialization
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_CONCEPT_HPP__