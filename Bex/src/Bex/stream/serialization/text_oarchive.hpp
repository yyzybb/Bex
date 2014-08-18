#ifndef __BEX_STREAM_SERIALIZATION_TEXT_OARCHIVE__
#define __BEX_STREAM_SERIALIZATION_TEXT_OARCHIVE__

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
        friend class base_serializer<text_oarchive>;

        std::streambuf& buf_;
        std::streamsize acc_;
        int deep_;

    public:
        explicit text_oarchive(std::streambuf& sb)
            : buf_(sb), acc_(0), deep_(0)
        {
        }

        explicit text_oarchive(std::ostream& os)
            : buf_(*os.rdbuf()), acc_(0), deep_(0)
        {
        }

        template <typename T>
        inline text_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline text_oarchive & operator<<(T const& t)
        {
            save(t);
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

    // @Todo: 改为private.
    public:
        using base_serializer<text_oarchive>::do_save;

        inline bool do_save(char const* buffer, std::size_t size)
        {
            std::streamsize ls = buf_.sputn(buffer, size);
            acc_ += ls;
            return (ls == size);
        }

        inline bool do_save(text_wrapper<char> & wrapper)
        {
            char & ch = wrapper.data();
            std::streamsize ls = buf_.sputn(&ch, 1);
            acc_ += ls;
            return (ls == 1);
        }

        template <typename T>
        inline bool do_save(text_wrapper<T> & wrapper)
        {
            BOOST_STATIC_ASSERT( (boost::is_arithmetic<T>::value) );

            std::string buffer;

            try {
                buffer = boost::lexical_cast<std::string>(wrapper.data()) + " ";
            } catch (...) { return false; }

            std::streamsize ls = buf_.sputn(buffer.c_str(), buffer.length());
            acc_ += ls;
            return (ls == buffer.length());
        }

    };

    //////////////////////////////////////////////////////////////////////////
    /// text_save
    template <typename T>
    bool text_save(T const& t, std::ostream& os)
    {
        boost::io::ios_flags_saver saver(os);
        os.unsetf(std::ios_base::skipws);
        text_oarchive bo(os);
        bo & t;
        return bo.good() ? true : (bo.clear(), false);
    }

    template <typename T>
    bool text_save(T const& t, std::streambuf& osb)
    {
        text_oarchive bo(osb);
        bo & t;
        return bo.good() ? true : (bo.clear(), false);
    }

    template <typename T>
    std::size_t text_save(T const& t, char * buffer, std::size_t size)
    {
        static_streambuf osb(buffer, size);
        text_oarchive bo(osb);
        bo & t;
        return bo.good() ? osb.size() : (bo.clear(), 0);
    }

    /// 数据持久化专用接口
    template <typename T, typename ... Args>
    FORCE_INLINE auto text_save_persistence(T const& t, Args && ... args) -> decltype(text_save(t, std::forward<Args>(args)...))
    {
        static_assert(has_serialize<T>::value, "The persistence data mustbe has serialize function.");
        static_assert(has_version<T>::value, "The persistence data mustbe has version.");
        return text_save(t, std::forward<Args>(args)...);
    }

} //namespace serialization

namespace {
    using serialization::text_oarchive;
    using serialization::text_save;
    using serialization::text_save_persistence;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_TEXT_OARCHIVE__