#ifndef __BEX_STREAM_SERIALIZATION_TEXT_IARCHIVE__
#define __BEX_STREAM_SERIALIZATION_TEXT_IARCHIVE__

#include <iosfwd>
#include "base_serializer.hpp"
#include <boost/lexical_cast.hpp>

//////////////////////////////////////////////////////////////////////////
/// 文本式序列化 in

namespace Bex { namespace serialization
{
    class text_iarchive
        : public base_serializer<text_iarchive>
        , public text_base
        , public input_archive_base
    {
        friend class rollback_sentry;

        std::streambuf& m_sb;
        archive_mark m_state;
        std::streamsize m_acc;

    public:
        explicit text_iarchive(std::streambuf& sb, archive_mark state = default_mark)
            : m_sb(sb), m_state(state), m_acc(0)
        {
        }

        explicit text_iarchive(std::istream& is, archive_mark state = default_mark)
            : m_sb(*is.rdbuf()), m_state(state), m_acc(0)
        {
        }

        using base_serializer<text_iarchive>::load;

        inline bool load(char * buffer, std::size_t size)
        {
            rollback_sentry sentry(this);
            std::streamsize ls = m_sb.sgetn(buffer, size);
            m_acc += ls;
            return sentry.wrap(ls == size);
        }

        inline bool load(text_wrapper<char> & wrapper)
        {
            rollback_sentry sentry(this);
            char & ch = wrapper.data();
            std::streamsize ls = m_sb.sgetn(&ch, 1);
            m_acc += ls;
            return sentry.wrap(ls == 1);
        }

        template <typename T>
        inline bool load(text_wrapper<T> & wrapper)
        {
            BOOST_STATIC_ASSERT( (boost::is_arithmetic<T>::value) );

            T & t = wrapper.data();
            try
            {
                rollback_sentry sentry(this);
                std::istreambuf_iterator<char> input_first(&m_sb);
                std::istreambuf_iterator<char> input_last;
                std::string buffer;
                std::streamsize ls = 0;
                bool bOk = false;
                while (input_first != input_last)
                {
                    ++ls;
                    char ch = *input_first++;
                    if (ch == ' ')
                    {
                        bOk = true;
                        break;
                    }

                    buffer += ch;
                }
                m_acc += ls;
                t = boost::lexical_cast<T>(buffer);
                return sentry.wrap(bOk);
            }
            catch(std::exception &)
            {
                return false;
            }
        }

        template <typename T>
        inline text_iarchive & operator&(T & t)
        {
            return (*this >> t);
        }

        template <typename T>
        inline text_iarchive & operator>>(T & t)
        {
            rollback_sentry sentry(this);
            if (!sentry.wrap(load(t)))
                throw exception("input error!");

            return (*this);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// text_load
    template <typename T>
    bool text_load(T & t, std::istream& is, archive_mark state = default_mark)
    {
        try
        {
            boost::io::ios_flags_saver saver(is);
            is.unsetf(std::ios_base::skipws);
            text_iarchive bi(is, state);
            bi & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    bool text_load(T & t, std::streambuf& isb, archive_mark state = default_mark)
    {
        try
        {
            text_iarchive bi(isb, state);
            bi & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    std::size_t text_load(T & t, char const * buffer, std::size_t size, archive_mark state = default_mark)
    {
        try
        {
            static_streambuf isb(const_cast<char*>(buffer), size, false);
            text_iarchive bi(isb, state);
            bi & t;
            return (isb.capacity() - isb.size());
        }
        catch (std::exception&)
        {
            return (std::size_t)0;
        }
    }

} //namespace serialization

namespace {
    using serialization::text_iarchive;
    using serialization::text_load;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_TEXT_IARCHIVE__