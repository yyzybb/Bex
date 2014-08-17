#ifndef __BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__

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
        friend class sentry;

        std::streambuf& buf_;
        archive_mark mark_;
        std::streamsize acc_;

    public:
        explicit binary_oarchive(std::streambuf& sb, archive_mark state = default_mark)
            : buf_(sb), mark_(state), acc_(0)
        {
        }

        explicit binary_oarchive(std::ostream& os, archive_mark state = default_mark)
            : buf_(*os.rdbuf()), mark_(state), acc_(0)
        {
        }

        inline bool save(char const* buffer, std::size_t size)
        {
            sentry sentry(this);
            std::streamsize ls = buf_.sputn(buffer, size);
            acc_ += ls;
            return sentry.wrap(ls == size);
        }

        inline bool save(binary_wrapper & wrapper)
        {
            return save(wrapper.data(), wrapper.size());
        }

        template <typename T>
        inline typename boost::disable_if<is_optimize<T, binary_oarchive>, bool>::type save(T const& t)
        {
            return base_serializer<binary_oarchive>::save(t);
        }

        template <typename T>
        inline typename boost::enable_if<is_optimize<T, binary_oarchive>, bool>::type save(T const& t)
        {
            return save((char const*)&t, sizeof(T));
        }

        template <typename T>
        inline binary_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline binary_oarchive & operator<<(T const& t)
        {
            sentry sentry(this);
            sentry.wrap(save(const_cast<T&>(t)));
            return (*this);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_save
    template <typename T>
    bool binary_save(T const& t, std::ostream& os, archive_mark state = default_mark)
    {
        boost::io::ios_flags_saver saver(os);
        os.unsetf(std::ios_base::skipws);
        binary_oarchive bo(os, state);
        bo & t;
        return bo.good();
    }

    template <typename T>
    bool binary_save(T const& t, std::streambuf& osb, archive_mark state = default_mark)
    {
        binary_oarchive bo(osb, state);
        bo & t;
        return bo.good();
    }

    template <typename T>
    std::size_t binary_save(T const& t, char * buffer, std::size_t size, archive_mark state = default_mark)
    {
        static_streambuf osb(buffer, size);
        binary_oarchive bo(osb, state);
        bo & t;
        return bo.good() ? osb.size() : 0;
    }

    /// 数据持久化专用接口
    template <typename T, typename ... Args>
    FORCE_INLINE auto binary_save_persistence(T const& t, Args && ... args) -> decltype(binary_save(t, std::forward<Args>(args)...))
    {
        static_assert(has_serialize<T>::value, "The persistence data mustbe has serialize function.");
        static_assert(has_version<T>::value, "The persistence data mustbe has version.");
        return binary_save(t, std::forward<Args>(args)...);
    }

} //namespace serialization

namespace {
    using serialization::binary_oarchive;
    using serialization::binary_save;
    using serialization::binary_save_persistence;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__