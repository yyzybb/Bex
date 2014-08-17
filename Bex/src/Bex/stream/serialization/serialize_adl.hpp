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

        template <class Archive, typename T>
        inline void mode_serialize(Archive & ar, T & t, binary_mode_tag)
        {
            binary_wrapper bwrap(t);
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
        inline typename boost::enable_if<detail::has_mem_serialize<T>, void>::type
        mem_serialize(Archive & ar, T & t, const unsigned int version)
        {
            t.serialize(ar, version);
        }

        template <class Archive, typename T>
        inline typename boost::disable_if<detail::has_mem_serialize<T>, void>::type
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
        unsigned int version = get_version<T>();
        serialize_with_version(ar, t, version
            , typename boost::mpl::if_c<has_version<T>::value
                , detail::has_version_tag, detail::no_version_tag>::type());
    }

} //namespace serialization
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_SERIALIZE_ADL__