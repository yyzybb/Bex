#ifndef __BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__

#include <iosfwd>
#include "base_serializer.hpp"

//////////////////////////////////////////////////////////////////////////
/// 序列化 out

namespace Bex { namespace serialization
{
    class binary_oarchive
        : public base_serializer<binary_oarchive>
        , public binary_base
        , public output_archive_base
    {
        friend class rollback_sentry;

        std::streambuf& m_sb;
        archive_mark m_state;
        std::streamsize m_acc;

    public:
        explicit binary_oarchive(std::streambuf& sb, archive_mark state = default_mark)
            : m_sb(sb), m_state(state), m_acc(0)
        {
        }

        explicit binary_oarchive(std::ostream& os, archive_mark state = default_mark)
            : m_sb(*os.rdbuf()), m_state(state), m_acc(0)
        {
        }

        using base_serializer<binary_oarchive>::save;

        inline bool save(char const* buffer, std::size_t size)
        {
            rollback_sentry sentry(this);
            std::streamsize ls = m_sb.sputn(buffer, size);
            m_acc += ls;
            return sentry.wrap(ls == size);
        }

        inline bool save(binary_wrapper & wrapper)
        {
            return save(wrapper.data(), wrapper.size());
        }

        template <typename T>
        inline binary_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline binary_oarchive & operator<<(T const& t)
        {
            rollback_sentry sentry(this);
            if (!sentry.wrap(save(const_cast<T&>(t))))
                throw exception("output error!");

            return (*this);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_save
    template <typename T>
    bool binary_save(T & t, std::ostream& os, archive_mark state = default_mark)
    {
        try
        {
            boost::io::ios_flags_saver saver(os);
            os.unsetf(std::ios_base::skipws);
            binary_oarchive bo(os, state);
            bo & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    bool binary_save(T & t, std::streambuf& osb, archive_mark state = default_mark)
    {
        try
        {
            binary_oarchive bo(osb, state);
            bo & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    std::size_t binary_save(T & t, char * buffer, std::size_t size, archive_mark state = default_mark)
    {
        try
        {
            static_streambuf osb(buffer, size);
            binary_oarchive bo(osb, state);
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
    using serialization::binary_oarchive;
    using serialization::binary_save;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__