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
        friend class sentry;

        std::streambuf& buf_;
        archive_mark mark_;
        std::streamsize acc_;

    public:
        explicit binary_iarchive(std::streambuf& sb, archive_mark state = default_mark)
            : buf_(sb), mark_(state), acc_(0)
        {
        }

        explicit binary_iarchive(std::istream& is, archive_mark state = default_mark)
            : buf_(*is.rdbuf()), mark_(state), acc_(0)
        {
        }

        inline bool load(char * buffer, std::size_t size)
        {
            sentry sentry(this);
            std::streamsize ls = buf_.sgetn(buffer, size);
            acc_ += ls;
            return sentry.wrap(ls == size);
        }

        inline bool load(binary_wrapper wrapper)
        {
            return load(wrapper.data(), wrapper.size());
        }

        template <typename T>
        inline typename boost::disable_if<is_optimize<typename boost::remove_reference<T>::type, binary_iarchive>, bool>::type load(T && t)
        {
            return base_serializer<binary_iarchive>::load(t);
        }

        template <typename T>
        inline typename boost::enable_if<is_optimize<typename boost::remove_reference<T>::type, binary_iarchive>, bool>::type load(T && t)
        {
            return load((char *)&t, sizeof(T));
        }

        template <typename T>
        inline binary_iarchive & operator&(T && t)
        {
            return (*this >> std::forward<T>(t));
        }

        template <typename T>
        inline binary_iarchive & operator>>(T && t)
        {
            sentry sentry(this);
            sentry.wrap(load(std::forward<T>(t)));
            return (*this);
        }

    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_load
    template <typename T>
    bool binary_load(T && t , std::istream& is, archive_mark state = default_mark)
    {
        boost::io::ios_flags_saver saver(is);
        is.unsetf(std::ios_base::skipws);
        binary_iarchive bi(is, state);
        bi & std::forward<T>(t);
        return bi.good();
    }

    template <typename T>
    bool binary_load(T && t , std::streambuf& isb, archive_mark state = default_mark)
    {
        binary_iarchive bi(isb, state);
        bi & std::forward<T>(t);
        return bi.good();
    }

    template <typename T>
    std::size_t binary_load(T && t , char const * buffer, std::size_t size, archive_mark state = default_mark)
    {
        static_streambuf isb(const_cast<char*>(buffer), size, false);
        binary_iarchive bi(isb, state);
        bi & std::forward<T>(t);
        return bi.good() ? (isb.capacity() - isb.size()) : 0;
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