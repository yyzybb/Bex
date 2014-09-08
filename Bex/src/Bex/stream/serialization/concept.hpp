#ifndef __BEX_STREAM_SERIALIZATION_CONCEPT_HPP__
#define __BEX_STREAM_SERIALIZATION_CONCEPT_HPP__

#include "serialization_fwd.h"
#include "version_adl.hpp"
#include "serialize_adl.hpp"

namespace Bex { namespace serialization
{
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
