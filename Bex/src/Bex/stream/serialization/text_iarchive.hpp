#ifndef __BEX_STREAM_SERIALIZATION_TEXT_IARCHIVE__
#define __BEX_STREAM_SERIALIZATION_TEXT_IARCHIVE__

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
        friend class base_serializer<text_iarchive>;

        std::streambuf& buf_;
        std::streamsize acc_;
        int deep_;

    public:
        explicit text_iarchive(std::streambuf& sb)
            : buf_(sb), acc_(0), deep_(0)
        {
        }

        explicit text_iarchive(std::istream& is)
            : buf_(*is.rdbuf()), acc_(0), deep_(0)
        {
        }

        template <typename T>
        inline text_iarchive & operator&(T && t)
        {
            return (*this >> std::forward<T>(t));
        }

        template <typename T>
        inline text_iarchive & operator>>(T && t)
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

    // @Todo: 改为private.
    public:
        using base_serializer<text_iarchive>::do_load;

        inline bool do_load(char * buffer, std::size_t size)
        {
            std::streamsize ls = buf_.sgetn(buffer, size);
            acc_ += ls;
            return (ls == size);
        }

        inline bool do_load(text_wrapper<char> & wrapper)
        {
            char & ch = wrapper.data();
            std::streamsize ls = buf_.sgetn(&ch, 1);
            acc_ += ls;
            return (ls == 1);
        }

        template <typename T>
        inline bool do_load(text_wrapper<T> & wrapper)
        {
            BOOST_STATIC_ASSERT( (boost::is_arithmetic<T>::value) );

            T & t = wrapper.data();

            std::istreambuf_iterator<char> input_first(&buf_);
            std::istreambuf_iterator<char> input_last;
            std::string buffer;
            std::streamsize ls = 0;
            while (input_first != input_last)
            {
                ++ls;
                char ch = *input_first++;
                if (ch == ' ')
                    break;

                buffer += ch;
            }

            if (ls)
            {
                try {
                    acc_ += ls;
                    t = boost::lexical_cast<T>(buffer);
                    return true;
                } catch (...) { return false; }
            }

            return false;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// text_load
    template <typename T>
    bool text_load(T && t , std::istream& is)
    {
        boost::io::ios_flags_saver saver(is);
        is.unsetf(std::ios_base::skipws);
        text_iarchive bi(is);
        bi & std::forward<T>(t);
        return bi.good() ? true : (bi.clear(), false);
    }

    template <typename T>
    bool text_load(T && t , std::streambuf& isb)
    {
        text_iarchive bi(isb);
        bi & std::forward<T>(t);
        return bi.good() ? true : (bi.clear(), false);
    }

    template <typename T>
    std::size_t text_load(T && t , char const * buffer, std::size_t size)
    {
        static_streambuf isb(const_cast<char*>(buffer), size, false);
        text_iarchive bi(isb);
        bi & std::forward<T>(t);
        return bi.good() ? (isb.capacity() - isb.size()) : (bi.clear(), 0);
    }

    /// 数据持久化专用接口
    template <typename T, typename ... Args>
    FORCE_INLINE auto text_load_persistence(T && t, Args && ... args)
        -> decltype(text_load(std::forward<T>(t), std::forward<Args>(args)...))
    {
        static_assert(has_serialize<T>::value, "The persistence data mustbe has serialize function.");
        static_assert(has_version<T>::value, "The persistence data mustbe has version.");
        return text_load(std::forward<T>(t), std::forward<Args>(args)...);
    }

} //namespace serialization

namespace {
    using serialization::text_iarchive;
    using serialization::text_load;
    using serialization::text_load_persistence;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_TEXT_IARCHIVE__