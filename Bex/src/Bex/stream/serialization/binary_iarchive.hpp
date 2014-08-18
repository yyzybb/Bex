#ifndef __BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_IARCHIVE__

#include "base_serializer.hpp"
#include <Bex/stream/fast_buffer.hpp>

//////////////////////////////////////////////////////////////////////////
/// 序列化 in

namespace Bex { namespace serialization
{
    template <typename Stream = std::streambuf>
    class binary_iarchive;

    template <>
    class binary_iarchive<std::streambuf>
        : public base_serializer<binary_iarchive<std::streambuf>>
        , public binary_base
        , public input_archive_base
    {
        friend class base_serializer<binary_iarchive<std::streambuf>>;

        std::streambuf& buf_;
        std::streamsize acc_;
        int deep_;

    public:
        explicit binary_iarchive(std::streambuf& sb)
            : buf_(sb), acc_(0), deep_(0)
        {
        }

        explicit binary_iarchive(std::istream& is)
            : buf_(*is.rdbuf()), acc_(0), deep_(0)
        {
        }

        template <typename T>
        inline binary_iarchive & operator&(T && t)
        {
            return (*this >> std::forward<T>(t));
        }

        template <typename T>
        inline binary_iarchive & operator>>(T && t)
        {
            load(std::forward<T>(t));
            return (*this);
        }

        template <typename T>
        inline bool load(T && t)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_load(t))
            {
                state_ = archive_state::error;
                -- deep_;
                return false;
            }
            else
            {
                if (!deep)
                    acc_ = 0;
                -- deep_;
                return true;
            }
        }

        inline bool load(char * buffer, std::size_t size)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_load(buffer, size))
            {
                state_ = archive_state::error;
                -- deep_;
                return false;
            }
            else
            {
                if (!deep)
                    acc_ = 0;
                -- deep_;
                return true;
            }
        }

    // @Todo: 改为private.
    public:
        inline bool do_load(char * buffer, std::size_t size)
        {
            std::streamsize ls = buf_.sgetn(buffer, size);
            acc_ += ls;
            return (ls == size);
        }

        template <typename T>
        inline typename boost::disable_if<is_optimize<typename boost::remove_reference<T>::type, binary_iarchive>, bool>::type do_load(T && t)
        {
            return base_serializer<binary_iarchive>::do_load(t);
        }

        template <typename T>
        inline typename boost::enable_if<is_optimize<typename boost::remove_reference<T>::type, binary_iarchive>, bool>::type do_load(T && t)
        {
            return do_load((char*)boost::addressof(t), sizeof(typename boost::remove_reference<T>::type));
        }
    };

    template <>
    class binary_iarchive<fast_buffer>
        : public base_serializer<binary_iarchive<fast_buffer>>
        , public binary_base
        , public input_archive_base
    {
        friend class base_serializer<binary_iarchive<fast_buffer>>;

        fast_buffer& buf_;
        fast_buffer::offset_type acc_;
        int deep_;

    public:
        explicit binary_iarchive(fast_buffer& sb)
            : buf_(sb), acc_(0), deep_(0)
        {
        }

        template <typename T>
        inline binary_iarchive & operator&(T && t)
        {
            return (*this >> std::forward<T>(t));
        }

        template <typename T>
        inline binary_iarchive & operator>>(T && t)
        {
            load(std::forward<T>(t));
            return (*this);
        }

        template <typename T>
        inline bool load(T && t)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_load(t))
            {
                state_ = archive_state::error;
                -- deep_;
                return false;
            }
            else
            {
                if (!deep)
                    acc_ = 0;
                -- deep_;
                return true;
            }
        }

        inline bool load(char * buffer, std::size_t size)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_load(buffer, size))
            {
                state_ = archive_state::error;
                -- deep_;
                return false;
            }
            else
            {
                if (!deep)
                    acc_ = 0;
                -- deep_;
                return true;
            }
        }

    // @Todo: 改为private.
    public:
        inline bool do_load(char * buffer, std::size_t size)
        {
            std::size_t ls = buf_.sgetn(buffer, size);
            acc_ += ls;
            return (ls == size);
        }

        template <typename T>
        inline typename boost::disable_if<is_optimize<typename boost::remove_reference<T>::type, binary_iarchive>, bool>::type do_load(T && t)
        {
            return base_serializer<binary_iarchive>::do_load(t);
        }

        template <typename T>
        inline typename boost::enable_if<is_optimize<typename boost::remove_reference<T>::type, binary_iarchive>, bool>::type do_load(T && t)
        {
            bool result = buf_.get(t);
            if (result) acc_ += sizeof(typename boost::remove_reference<T>::type);
            return result;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_load
    template <typename T>
    bool binary_load(T && t , std::istream& is)
    {
        boost::io::ios_flags_saver saver(is);
        is.unsetf(std::ios_base::skipws);
        binary_iarchive<> bi(is);
        bi & std::forward<T>(t);
        return bi.good() ? true : (bi.clear(), false);
    }

    template <typename T>
    bool binary_load(T && t , std::streambuf& isb)
    {
        binary_iarchive<> bi(isb);
        bi & std::forward<T>(t);
        return bi.good() ? true : (bi.clear(), false);
    }

    template <typename T>
    std::size_t binary_load(T && t , char const * buffer, std::size_t size)
    {
        fast_buffer isb(const_cast<char*>(buffer), size);
        isb.pbump(isb.capacity());
        binary_iarchive<fast_buffer> bi(isb);
        bi & std::forward<T>(t);
        return bi.good() ? (isb.capacity() - isb.gcount()) : 0;
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