#ifndef __BEX_STREAM_SERIALIZATION_BASE_SERIALIZER__
#define __BEX_STREAM_SERIALIZATION_BASE_SERIALIZER__

#include "serialization_fwd.h"
#include "concept.hpp"
#include "archive_traits.hpp"
#include "serialize_adl.hpp"

//////////////////////////////////////////////////////////////////////////
/// 序列化接口基类

namespace Bex { namespace serialization
{
    template <class Archive>
    class base_serializer
    {
        inline Archive & get()
        {
            return static_cast<Archive&>(*this);
        }

    protected:
        template <class Ar, typename T>
        struct adapter
        {
            inline static bool do_save(T & t, Ar & ar)
            {
                serialize_adl(ar, t);
                return ar.good();
            }

            inline static bool do_load(T & t, Ar & ar)
            {
                serialize_adl(ar, t);
                return ar.good();
            }
        };

        template <class Ar, typename T>
        struct adapter<Ar, T*>
        {
            inline static bool do_save(T * pt, Ar & ar)
            {
                BOOST_ASSERT(pt != 0);
                return adapter<Ar, T>::do_save(*pt, ar);
            }

            inline static bool do_load(T * pt, Ar & ar)
            {
                BOOST_ASSERT(pt != 0);
                return adapter<Ar, T>::do_load(*pt, ar);
            }
        };

#include "stl_adapter.hpp"
#include "array_adapter.hpp"
#include "boost_container_adapter.hpp"

    public:
        base_serializer() : state_(archive_state::good) {}

    protected:
        //////////////////////////////////////////////////////////////////////////
        /// save
        template <typename T>
        inline bool do_save(T const& t)
        {
            return adapter<Archive, T>::do_save(const_cast<T&>(t), get());
        }

        //////////////////////////////////////////////////////////////////////////
        /// load
        template <typename T>
        inline bool do_load(T && t)
        {
            return adapter<Archive, typename boost::remove_reference<T>::type>::do_load(t, get());
        }

    public:
        inline bool good() const
        {
            return state_ != archive_state::error;
        }

        inline void clear(bool rollback = true)
        {
            if (!good())
            {
                state_ = archive_state::good;
                if (rollback)
                    get().buf_.pubseekoff(-get().acc_, std::ios_base::cur, mode(typename archive_traits<Archive>::oper_tag()));
                get().acc_ = 0;
            }
        }

    protected:
        archive_state state_;

        inline std::ios_base::openmode mode(load_oper_tag)
        {
            return std::ios_base::in;
        }

        inline std::ios_base::openmode mode(save_oper_tag)
        {
            return std::ios_base::out;
        }

        inline std::ios_base::openmode mode(unkown_oper_tag)
        {
            return std::ios_base::in | std::ios_base::out;
        }
    };

} //namespace serialization
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BASE_SERIALIZER__