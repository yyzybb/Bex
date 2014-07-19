#ifndef __BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__
#define __BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__

#include <Bex/math/compress_numeric.hpp>
#include <boost/type_traits/is_class.hpp>
#include "utility.hpp"

namespace Bex { namespace serialization
{
    namespace detail
    {
        struct mem_serialize_tag {};
        struct no_mem_serialize_tag {};

        struct has_version_tag {};
        struct no_version_tag {};

        template <class Archive, typename T>
        inline void mode_serialize(Archive & ar, T & t, binary_mode_tag)
        {
            binary_wrapper bwrap((char*)&t, sizeof(T));
            ar & bwrap;
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
        inline void mem_serialize(Archive & ar, T & t, const unsigned int version, mem_serialize_tag)
        {
            t.serialize(ar, version);
        }

        template <class Archive, typename T>
        inline void mem_serialize(Archive & ar, T & t, const unsigned int, no_mem_serialize_tag)
        {
            mode_serialize(ar, t, typename archive_traits<Archive>::mode_tag());
        }
    } //namespace detail

#if defined(BOOST_NO_CXX11_DECLTYPE)
    template <class Archive, typename T>
    inline void serialize(Archive & ar, T & t, const unsigned int version)
    {
        detail::mem_serialize(ar, t, version
            , typename boost::mpl::if_c<has_serialize<T>::value
                , detail::mem_serialize_tag, detail::no_mem_serialize_tag>::type());
    }
#else
    class private_type_wrapper
    {
        struct type {};

        template <class Archive, typename T>
        friend type serialize(Archive &, T &, const unsigned int);
    };

    template <class Archive, typename T>
    inline private_type_wrapper::type serialize(Archive & ar, T & t, const unsigned int version)
    {
        detail::mem_serialize(ar, t, version
            , typename boost::mpl::if_c<has_serialize<T>::value
                , detail::mem_serialize_tag, detail::no_mem_serialize_tag>::type());

        return private_type_wrapper::type();
    }
#endif

    template <class Archive, typename T>
    inline void version_serialize(Archive & ar, T & t, const unsigned int version, detail::no_version_tag)
    {
        serialize(ar, t, version);
    }

    template <class Archive, typename T>
    inline void version_serialize(Archive & ar, T & t, const unsigned int version, detail::has_version_tag)
    {
        cn32 ver(version);
        ar & ver;
        version_serialize(ar, t, ver.get(), detail::no_version_tag());
    }

    template <class Archive, typename T>
    inline void serialize_adl(Archive & ar, T & t, const unsigned int version)
    {
        version_serialize(ar, t, version
            , typename boost::mpl::if_c<has_version<T>::value
                , detail::has_version_tag, detail::no_version_tag>::type());
    }
} //namespace serialization

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__