#ifndef __BEX_STREAM_SERIALIZATION_BASE_SERIALIZER__
#define __BEX_STREAM_SERIALIZATION_BASE_SERIALIZER__

#include <Bex/config.hpp>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/io/ios_state.hpp>
#include <Bex/utility/exception.h>
#include "utility.hpp"
#include "serialize_adl.hpp"

//////////////////////////////////////////////////////////////////////////
/// 序列化接口基类

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
# define BEX_SERIALIZATION_INTERFACE_REFERENCE(type) type &
#else
# define BEX_SERIALIZATION_INTERFACE_REFERENCE(type) type &&
#endif //defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

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
                try
                {
                    serialize_adl(ar, t, get_version<T>::value);
                    return true;
                }
                catch(std::exception&)
                {
                    return false;
                }
            }

            inline static bool load(T & t, Ar & ar)
            {
                try
                {
                    serialize_adl(ar, t, get_version<T>::value);
                    return true;
                }
                catch(std::exception&)
                {
                    return false;
                }
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
        //////////////////////////////////////////////////////////////////////////
        /// save
        template <typename T>
        inline bool save(T const& t)
        {
            rollback_sentry sentry(get());
            return sentry.wrap(adapter<Archive, T>::save(const_cast<T&>(t), get()));
        }

        //////////////////////////////////////////////////////////////////////////
        /// load
        template <typename T>
        inline bool load(BEX_SERIALIZATION_INTERFACE_REFERENCE(T) t)
        {
            rollback_sentry sentry(get());
            return sentry.wrap(adapter<Archive, typename boost::remove_reference<T>::type 
                >::load(t, get()));
        }

    protected:
        class rollback_sentry
        {
            typedef archive_traits<Archive> traits;

            Archive & m_ar;
            bool m_rollback;

        public:
            explicit rollback_sentry(Archive & ar)
                : m_ar(ar), m_rollback(ar.m_acc == 0)
            {
            }

            explicit rollback_sentry(Archive * pAr)
                : m_ar(*pAr), m_rollback(pAr->m_acc == 0)
            {
            }

            inline bool wrap(bool bOk)
            {
                m_rollback = m_rollback && !bOk;
                return bOk;
            }

            ~rollback_sentry()
            {
                if (m_rollback && m_ar.m_acc > 0)
                {
                    if (m_ar.m_state & amb_rollback)
                        m_ar.m_sb.pubseekoff( -m_ar.m_acc, std::ios_base::cur
                            , mode(typename traits::oper_tag()));
                    m_ar.m_acc = 0;
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