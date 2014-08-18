#ifndef __BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_OARCHIVE__

#include "base_serializer.hpp"
#include <Bex/stream/fast_buffer.hpp>

//////////////////////////////////////////////////////////////////////////
/// 序列化 out

namespace Bex { namespace serialization
{
    template <typename Stream = std::streambuf>
    class binary_oarchive;

    template <>
    class binary_oarchive<std::streambuf>
        : public base_serializer<binary_oarchive<std::streambuf>>
        , public binary_base
        , public output_archive_base
    {
        friend class base_serializer<binary_oarchive<std::streambuf>>;

        std::streambuf& buf_;
        std::streamsize acc_;
        int deep_;

    public:
        explicit binary_oarchive(std::streambuf& sb)
            : buf_(sb), acc_(0), deep_(0)
        {
        }

        explicit binary_oarchive(std::ostream& os)
            : buf_(*os.rdbuf()), acc_(0), deep_(0)
        {
        }

        template <typename T>
        inline binary_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline binary_oarchive & operator<<(T const& t)
        {
            save(const_cast<T&>(t));
            return (*this);
        }

        template <typename T>
        inline bool save(T const& t)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_save(const_cast<T&>(t)))
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

        inline bool save(char const* buffer, std::size_t size)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_save(buffer, size))
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
        inline bool do_save(char const* buffer, std::size_t size)
        {
            std::streamsize ls = buf_.sputn(buffer, size);
            acc_ += ls;
            return (ls == size);
        }

        template <typename T>
        inline typename boost::disable_if<is_optimize<T, binary_oarchive>, bool>::type do_save(T const& t)
        {
            return base_serializer<binary_oarchive>::do_save(t);
        }

        template <typename T>
        inline typename boost::enable_if<is_optimize<T, binary_oarchive>, bool>::type do_save(T const& t)
        {
            return do_save((char const*)boost::addressof(t), sizeof(T));
        }
    };


    template <>
    class binary_oarchive<fast_buffer>
        : public base_serializer<binary_oarchive<fast_buffer>>
        , public binary_base
        , public output_archive_base
    {
        friend class base_serializer<binary_oarchive<fast_buffer>>;

        fast_buffer& buf_;
        fast_buffer::offset_type acc_;
        int deep_;

    public:
        explicit binary_oarchive(fast_buffer& sb)
            : buf_(sb), acc_(0), deep_(0)
        {
        }

        template <typename T>
        inline binary_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline binary_oarchive & operator<<(T const& t)
        {
            save(const_cast<T&>(t));
            return (*this);
        }

        template <typename T>
        inline bool save(T const& t)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_save(const_cast<T&>(t)))
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

        inline bool save(char const* buffer, std::size_t size)
        {
            if (!good()) return false;

            int deep = deep_++;
            if (!do_save(buffer, size))
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
        inline bool do_save(char const* buffer, std::size_t size)
        {
            std::size_t ls = buf_.sputn(buffer, size);
            acc_ += ls;
            return (ls == size);
        }

        template <typename T>
        inline typename boost::disable_if<is_optimize<T, binary_oarchive>, bool>::type do_save(T const& t)
        {
            return base_serializer<binary_oarchive>::do_save(t);
        }

        template <typename T>
        inline typename boost::enable_if<is_optimize<T, binary_oarchive>, bool>::type do_save(T const& t)
        {
            bool result = buf_.put(t);
            if (result) acc_ += sizeof(T);
            return result;
        }

    };

    //////////////////////////////////////////////////////////////////////////
    /// binary_save
    template <typename T>
    bool binary_save(T const& t, std::ostream& os)
    {
        boost::io::ios_flags_saver saver(os);
        os.unsetf(std::ios_base::skipws);
        binary_oarchive<> bo(os);
        bo & t;
        return bo.good() ? true : (bo.clear(), false);
    }

    template <typename T>
    bool binary_save(T const& t, std::streambuf& osb)
    {
        binary_oarchive<> bo(osb);
        bo & t;
        return bo.good() ? true : (bo.clear(), false);
    }

    template <typename T>
    std::size_t binary_save(T const& t, char * buffer, std::size_t size)
    {
        fast_buffer osb(buffer, size);
        binary_oarchive<fast_buffer> bo(osb);
        bo & t;
        return bo.good() ? osb.gcount() : (bo.clear(), 0);
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