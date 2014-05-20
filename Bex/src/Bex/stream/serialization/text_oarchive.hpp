#ifndef __BEX_STREAM_SERIALIZATION_TEXT_OARCHIVE__
#define __BEX_STREAM_SERIALIZATION_TEXT_OARCHIVE__

#include <iosfwd>
#include "base_serializer.hpp"
#include <boost/lexical_cast.hpp>

//////////////////////////////////////////////////////////////////////////
/// 文本式序列化 out

namespace Bex { namespace serialization
{
    class text_oarchive
        : public base_serializer<text_oarchive>
        , public text_base
        , public output_archive_base
    {
        friend class rollback_sentry;

        std::streambuf& m_sb;
        archive_mark m_state;
        std::streamsize m_acc;

    public:
        explicit text_oarchive(std::streambuf& sb, archive_mark state = default_mark)
            : m_sb(sb), m_state(state), m_acc(0)
        {
        }

        explicit text_oarchive(std::ostream& os, archive_mark state = default_mark)
            : m_sb(*os.rdbuf()), m_state(state), m_acc(0)
        {
        }

        using base_serializer<text_oarchive>::save;

        inline bool save(char const* buffer, std::size_t size)
        {
            rollback_sentry sentry(this);
            std::streamsize ls = m_sb.sputn(buffer, size);
            m_acc += ls;
            return sentry.wrap(ls == size);
        }

        inline bool save(text_wrapper<char> & wrapper)
        {
            rollback_sentry sentry(this);
            char & ch = wrapper.data();
            std::streamsize ls = m_sb.sputn(&ch, 1);
            m_acc += ls;
            return sentry.wrap(ls == 1);
        }

        template <typename T>
        inline bool save(text_wrapper<T> & wrapper)
        {
            BOOST_STATIC_ASSERT( (boost::is_arithmetic<T>::value) );

            try
            {
                rollback_sentry sentry(this);
                std::string buffer = boost::lexical_cast<std::string>(wrapper.data()) + " ";
                std::streamsize ls = m_sb.sputn(buffer.c_str(), buffer.length());
                m_acc += ls;
                return sentry.wrap(ls == buffer.length());
            }
            catch(std::exception &)
            {
                return false;
            }
        }

        template <typename T>
        inline text_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline text_oarchive & operator<<(T const& t)
        {
            rollback_sentry sentry(this);
            if (!sentry.wrap(save(const_cast<T&>(t))))
                throw exception("output error!");

            return (*this);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// text_save
    template <typename T>
    bool text_save(T & t, std::ostream& os, archive_mark state = default_mark)
    {
        try
        {
            boost::io::ios_flags_saver saver(os);
            os.unsetf(std::ios_base::skipws);
            text_oarchive bo(os, state);
            bo & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    bool text_save(T & t, std::streambuf& osb, archive_mark state = default_mark)
    {
        try
        {
            text_oarchive bo(osb, state);
            bo & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    std::size_t text_save(T & t, char * buffer, std::size_t size, archive_mark state = default_mark)
    {
        try
        {
            static_streambuf osb(buffer, size);
            text_oarchive bo(osb, state);
            bo & t;
            return osb.size();
        }
        catch (std::exception&)
        {
            return (std::size_t)0;
        }
    }

} //namespace serialization

namespace {
    using serialization::text_oarchive;
    using serialization::text_save;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_TEXT_OARCHIVE__