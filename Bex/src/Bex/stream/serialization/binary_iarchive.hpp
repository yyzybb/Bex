#ifndef __BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__

#include <iosfwd>
#include "base_serializer.hpp"

//////////////////////////////////////////////////////////////////////////
/// –Ú¡–ªØ in

namespace Bex { namespace serialization
{
    class binary_iarchive
        : public base_serializer<binary_iarchive>
        , public binary_base
        , public input_archive_base
    {
        friend class rollback_sentry;

        std::streambuf& m_sb;
        archive_mark m_state;
        std::streamsize m_acc;

    public:
        explicit binary_iarchive(std::streambuf& sb, archive_mark state = default_mark)
            : m_sb(sb), m_state(state), m_acc(0)
        {
        }

        explicit binary_iarchive(std::istream& is, archive_mark state = default_mark)
            : m_sb(*is.rdbuf()), m_state(state), m_acc(0)
        {
        }

        using base_serializer<binary_iarchive>::load;

        inline bool load(char * buffer, std::size_t size)
        {
            rollback_sentry sentry(this);
            std::streamsize ls = m_sb.sgetn(buffer, size);
            m_acc += ls;
            return sentry.wrap(ls == size);
        }

        inline bool load(binary_wrapper & wrapper)
        {
            return load(wrapper.data(), wrapper.size());
        }

        template <typename T>
        inline binary_iarchive & operator&(BEX_SERIALIZATION_INTERFACE_REFERENCE(T) t)
        {
            return (*this >> t);
        }

        template <typename T>
        inline binary_iarchive & operator>>(BEX_SERIALIZATION_INTERFACE_REFERENCE(T) t)
        {
            rollback_sentry sentry(this);
            if (!sentry.wrap(load(t)))
                throw exception("input error!");

            return (*this);
        }

    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_load
    template <typename T>
    bool binary_load(BEX_SERIALIZATION_INTERFACE_REFERENCE(T) t
        , std::istream& is, archive_mark state = default_mark)
    {
        try
        {
            boost::io::ios_flags_saver saver(is);
            is.unsetf(std::ios_base::skipws);
            binary_iarchive bi(is, state);
            bi & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    bool binary_load(BEX_SERIALIZATION_INTERFACE_REFERENCE(T) t
        , std::streambuf& isb, archive_mark state = default_mark)
    {
        try
        {
            binary_iarchive bi(isb, state);
            bi & t;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    std::size_t binary_load(BEX_SERIALIZATION_INTERFACE_REFERENCE(T) t
        , char const * buffer, std::size_t size, archive_mark state = default_mark)
    {
        try
        {
            static_streambuf isb(const_cast<char*>(buffer), size, false);
            binary_iarchive bi(isb, state);
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
    using serialization::binary_iarchive;
    using serialization::binary_load;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__