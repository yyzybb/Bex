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

        template <class Ar, typename T>
        struct adapter
        {
            inline static bool save(T & t, Ar & ar)
            {
                serialize_adl(ar, t);
                return ar.good();
            }

            inline static bool load(T & t, Ar & ar)
            {
                serialize_adl(ar, t);
                return ar.good();
            }
        };

        template <class Ar, typename T>
        struct adapter<Ar, T*>
        {
            inline static bool save(T * pt, Ar & ar)
            {
                BOOST_ASSERT(pt != 0);
                return adapter<Ar, T>::save(*pt, ar);
            }

            inline static bool load(T * pt, Ar & ar)
            {
                BOOST_ASSERT(pt != 0);
                return adapter<Ar, T>::load(*pt, ar);
            }
        };

#include "stl_adapter.hpp"
#include "array_adapter.hpp"
#include "boost_container_adapter.hpp"

    public:
        base_serializer() : state_(archive_state::idle) {}

        //////////////////////////////////////////////////////////////////////////
        /// save
        template <typename T>
        inline bool save(T const& t)
        {
            sentry sentry(get());
            return sentry.wrap(adapter<Archive, T>::save(const_cast<T&>(t), get()));
        }

        //////////////////////////////////////////////////////////////////////////
        /// load
        template <typename T>
        inline bool load(T && t)
        {
            sentry sentry(get());
            return sentry.wrap(adapter<Archive, typename boost::remove_reference<T>::type 
                >::load(t, get()));
        }

        inline bool good() const
        {
            return state_ != archive_state::error;
        }

        inline void clear()
        {
            if (!good()) state_ = archive_state::idle;
        }

    protected:
        archive_state state_;

        class sentry
        {
            typedef archive_traits<Archive> traits;

            Archive & ar_;
            bool rollback_;
            bool start_;

        public:
            explicit sentry(Archive & ar)
                : ar_(ar), rollback_(ar.acc_ == 0), start_(ar.state_ == archive_state::idle)
            {
                if (start_) ar_.state_ = archive_state::running;
            }

            explicit sentry(Archive * pAr)
                : ar_(*pAr), rollback_(pAr->acc_ == 0), start_(pAr->state_ == archive_state::idle)
            {
                if (start_) ar_.state_ = archive_state::running;
            }

            inline bool wrap(bool bOk)
            {
                rollback_ = rollback_ && !bOk;
                if (!bOk) ar_.state_ = archive_state::error;
                return bOk;
            }

            ~sentry()
            {
                if (rollback_ && ar_.acc_ > 0)
                {
                    if (ar_.mark_ & amb_rollback)
                        ar_.buf_.pubseekoff(-ar_.acc_, std::ios_base::cur
                        , mode(typename traits::oper_tag()));
                    ar_.acc_ = 0;
                    ar_.state_ = archive_state::error;
                }

                if (start_ && ar_.state_ != archive_state::error)
                {
                    ar_.state_ = archive_state::idle;
                    ar_.acc_ = 0;
                }
            }

        private:
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
    };

} //namespace serialization

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BASE_SERIALIZER__