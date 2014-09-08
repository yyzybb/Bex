#ifndef __BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__
#define __BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__

#include "serialization_fwd.h"
#include "version_adl.hpp"

namespace Bex { namespace serialization
{
    namespace detail
    {
        struct has_version_tag {};
        struct no_version_tag {};

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


        template <class Archive, typename T>
        inline void mode_serialize(Archive & ar, T & t, binary_mode_tag)
        {
            BOOST_ASSERT(false);
        }

        template <class Archive, typename T>
        inline void mode_serialize(Archive & ar, T & t, text_mode_tag)
        {
            text_wrapper<T> twrap(t);
            ar & twrap;
        }

        template <class Archive, typename T>
        inline void mode_serialize(Archive & ar, T & t, unkown_mode_tag)
        {
            //BOOST_STATIC_ASSERT(false);
        }

        template <class Archive, typename T>
        inline typename boost::enable_if<has_mem_serialize<T>, void>::type
        mem_serialize(Archive & ar, T & t, const unsigned int version)
        {
            t.serialize(ar, version);
        }

        template <class Archive, typename T>
        inline typename boost::disable_if<has_mem_serialize<T>, void>::type
        mem_serialize(Archive & ar, T & t, const unsigned int)
        {
            mode_serialize(ar, t, typename archive_traits<Archive>::mode_tag());
        }
    } //namespace detail

    class private_type_wrapper
    {
        struct type {};

        template <class Archive, typename T>
        friend type serialize(Archive &, T &, const unsigned int);
    };

    template <class Archive, typename T>
    inline private_type_wrapper::type serialize(Archive & ar, T & t, const unsigned int version)
    {
        detail::mem_serialize(ar, t, version);
        return private_type_wrapper::type();
    }

    template <class Archive, typename T>
    inline void serialize_with_version(Archive & ar, T & t, const unsigned int version, detail::no_version_tag)
    {
        serialize(ar, t, version);
    }

    template <class Archive, typename T>
    inline void serialize_with_version(Archive & ar, T & t, const unsigned int version, detail::has_version_tag)
    {
        cn32 ver(version);
        ar & ver;
        serialize_with_version(ar, t, ver.get(), detail::no_version_tag());
    }

    template <class Archive, typename T>
    inline void serialize_adl(Archive & ar, T & t)
    {
        serialize_with_version(ar, t, get_version<T>()
            , typename boost::mpl::if_c<has_version<T>::value
                , detail::has_version_tag, detail::no_version_tag>::type());
    }

    //////////////////////////////////////////////////////////////////////////
    /// @{ optimize
    namespace detail
    {
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

} //namespace serialization
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__
