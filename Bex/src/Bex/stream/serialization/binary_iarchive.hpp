#ifndef __BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__

#include "base_serializer.hpp"

//////////////////////////////////////////////////////////////////////////
/// 序列化 in

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

        inline bool load(binary_wrapper wrapper)
        {
            return load(wrapper.data(), wrapper.size());
        }

        template <typename T>
        inline binary_iarchive & operator&(T && t)
        {
            return (*this >> std::forward<T>(t));
        }

        template <typename T>
        inline binary_iarchive & operator>>(T && t)
        {
            rollback_sentry sentry(this);
            if (!sentry.wrap(load(std::forward<T>(t))))
                throw exception("input error!");

            return (*this);
        }

    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_load
    template <typename T>
    bool binary_load(T && t , std::istream& is, archive_mark state = default_mark)
    {
        try
        {
            boost::io::ios_flags_saver saver(is);
            is.unsetf(std::ios_base::skipws);
            binary_iarchive bi(is, state);
            bi & std::forward<T>(t);
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    bool binary_load(T && t , std::streambuf& isb, archive_mark state = default_mark)
    {
        try
        {
            binary_iarchive bi(isb, state);
            bi & std::forward<T>(t);
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    template <typename T>
    std::size_t binary_load(T && t , char const * buffer, std::size_t size, archive_mark state = default_mark)
    {
        try
        {
            static_streambuf isb(const_cast<char*>(buffer), size, false);
            binary_iarchive bi(isb, state);
            bi & std::forward<T>(t);
            return (isb.capacity() - isb.size());
        }
        catch (std::exception&)
        {
            return (std::size_t)0;
        }
    }

    /// 数据持久化专用接口
    template <typename T, typename ... Args>
    FORCE_INLINE auto binary_load_persistence(T && t, Args && ... args)
        -> decltype(binary_load(std::forward<T>(t), std::forward<Args>(args)...))
    {
        static_assert(has_serialize<T>::value, "The persistence data mustbe has serialize function.");
        static_assert(has_version<T>::value, "The persistence data mustbe has version.");
        return binary_load(std::forward<T>(t), std::forward<Args>(args)...);
    }

} //namespace serialization

namespace {
    using serialization::binary_iarchive;
    using serialization::binary_load;
    using serialization::binary_load_persistence;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__