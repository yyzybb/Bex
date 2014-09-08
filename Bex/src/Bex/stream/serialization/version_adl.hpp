#ifndef __BEX_STREAM_SERIALIZATION_VERSION_ADL_HPP__
#define __BEX_STREAM_SERIALIZATION_VERSION_ADL_HPP__

//////////////////////////////////////////////////////////////////////////
/// 用户自定义类型的版本号
#include "serialization_fwd.h"

namespace Bex { namespace serialization
{
    class serialize_version_helper
    {
        struct unsigned_int_t
        {
            unsigned int ui_;
            unsigned_int_t(unsigned int ui) : ui_(ui) {}
            operator unsigned int() const { return ui_; }
        };

        template <typename T>
        friend unsigned_int_t serialize_version(T*);
    };

    template <typename T>
    serialize_version_helper::unsigned_int_t serialize_version(T* pt)
    {
        return pt->serialize_version();
    }

    template <typename T>
    unsigned int serialize_version_adl(T* pt)
    {
        return serialize_version(pt);
    }

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

} //namespace serialization
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_VERSION_ADL_HPP__
