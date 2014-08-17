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
        friend class sentry;

        std::streambuf& buf_;
        archive_mark mark_;
        std::streamsize acc_;

    public:
        explicit text_oarchive(std::streambuf& sb, archive_mark state = default_mark)
            : buf_(sb), mark_(state), acc_(0)
        {
        }

        explicit text_oarchive(std::ostream& os, archive_mark state = default_mark)
            : buf_(*os.rdbuf()), mark_(state), acc_(0)
        {
        }

        using base_serializer<text_oarchive>::save;

        inline bool save(char const* buffer, std::size_t size)
        {
            sentry sentry(this);
            std::streamsize ls = buf_.sputn(buffer, size);
            acc_ += ls;
            return sentry.wrap(ls == size);
        }

        inline bool save(text_wrapper<char> & wrapper)
        {
            sentry sentry(this);
            char & ch = wrapper.data();
            std::streamsize ls = buf_.sputn(&ch, 1);
            acc_ += ls;
            return sentry.wrap(ls == 1);
        }

        template <typename T>
        inline bool save(text_wrapper<T> & wrapper)
        {
            BOOST_STATIC_ASSERT( (boost::is_arithmetic<T>::value) );

            sentry sentry(this);
            std::string buffer = boost::lexical_cast<std::string>(wrapper.data()) + " ";
            std::streamsize ls = buf_.sputn(buffer.c_str(), buffer.length());
            acc_ += ls;
            return sentry.wrap(ls == buffer.length());
        }

        template <typename T>
        inline text_oarchive & operator&(T const& t)
        {
            return (*this << t);
        }

        template <typename T>
        inline text_oarchive & operator<<(T const& t)
        {
            sentry sentry(this);
            if (!sentry.wrap(save(const_cast<T&>(t))))
                throw exception("output error!");

            return (*this);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// text_save
    template <typename T>
    bool text_save(T const& t, std::ostream& os, archive_mark state = default_mark)
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
    bool text_save(T const& t, std::streambuf& osb, archive_mark state = default_mark)
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
    std::size_t text_save(T const& t, char * buffer, std::size_t size, archive_mark state = default_mark)
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