#ifndef __BEX_STREAM_FAST_BUFFER_HPP__
#define __BEX_STREAM_FAST_BUFFER_HPP__

#include <iosfwd>
#include <algorithm>
#include <boost/type_traits.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/addressof.hpp>

namespace Bex { namespace stream
{
    class fast_buffer
    {
        char * buf_;
        std::size_t size_;
        char * put_;
        char * get_;

    public:
        typedef boost::make_signed<std::size_t>::type offset_type;

    public:
        fast_buffer(char * buf, std::size_t size)
            : buf_(buf), size_(size), put_(buf), get_(buf)
        {
            BOOST_ASSERT(buf);
        }

        template <int N>
        explicit fast_buffer(char (&arr)[N])
            : buf_(&arr[0]), size_(N), put_(&arr[0]), get_(&arr[0])
        {
            BOOST_STATIC_ASSERT_MSG(N > 0, "Buffer cannot empty!");
            BOOST_ASSERT(buf);
        }

        inline char * pptr()
        {
            return put_;
        }

        inline char * gptr()
        {
            return get_;
        }

        inline std::size_t pcount() const
        {
            return (buf_ + size_ - put_);
        }

        inline std::size_t gcount() const
        {
            return (put_ - get_);
        }

        inline std::size_t capacity() const
        {
            return size_;
        }

        inline std::size_t sputn(char const* data, std::size_t len)
        {
            if (pcount() < len) return 0;
            copy(put_, data, len);
            pbump(len);
            return len;
        }

        inline std::size_t sgetn(char * data, std::size_t len)
        {
            if (gcount() < len) return 0;
            copy(data, get_, len);
            gbump(len);
            return len;
        }

        template <typename T>
        typename boost::enable_if<std::is_pod<typename boost::remove_reference<T>::type>, bool>::type put(T const& t)
        {
            typedef typename boost::remove_reference<T>::type OriginType;
            if (pcount() < sizeof(OriginType)) return false;
            *(OriginType*)put_ = t;
            pbump(sizeof(OriginType));
            return true;
        }

        template <typename T>
        typename boost::disable_if<std::is_pod<typename boost::remove_reference<T>::type>, bool>::type put(T const& t)
        {
            return (sputn(boost::addressof(t), sizeof(T)) == sizeof(T));
        }

        template <typename T>
        typename boost::enable_if<std::is_pod<typename boost::remove_reference<T>::type>, bool>::type get(T && t)
        {
            typedef typename boost::remove_reference<T>::type OriginType;
            if (gcount() < sizeof(OriginType)) return false;
            t = *(OriginType*)get_;
            gbump(sizeof(OriginType));
            return true;
        }

        template <typename T>
        typename boost::disable_if<std::is_pod<typename boost::remove_reference<T>::type>, bool>::type get(T && t)
        {
            return (sgetn(boost::addressof(t), sizeof(T)) == sizeof(T));
        }

        inline void pubseekoff(offset_type offset, std::ios_base::seekdir sd, std::ios_base::openmode mode)
        {
            if (std::ios_base::cur == sd)
            {
                if (std::ios_base::in & mode)
                    gbump(offset);

                if (std::ios_base::out & mode)
                    pbump(offset);
            }
            else if (std::ios_base::beg == sd)
            {
                if (std::ios_base::in & mode)
                {
                    offset_type bumpoffset = offset + (get_ - buf_);
                    gbump(bumpoffset);
                }

                if (std::ios_base::out & mode)
                {
                    offset_type bumpoffset = offset + (put_ - buf_);
                    pbump(bumpoffset);
                }
            }
            else if (std::ios_base::end == sd)
            {
                if (std::ios_base::in & mode)
                {
                    offset_type bumpoffset = offset + (get_ - buf_ + size_);
                    gbump(bumpoffset);
                }

                if (std::ios_base::out & mode)
                {
                    offset_type bumpoffset = offset + (put_ - buf_ + size_);
                    pbump(bumpoffset);
                }
            }
        }

        inline void pbump(offset_type n)
        {
            BOOST_ASSERT((put_ + n <= buf_ + size_));
            put_ += n;
        }

        inline void gbump(offset_type n)
        {
            BOOST_ASSERT((get_ + n <= buf_ + size_));
            get_ += n;
        }

    private:
        inline void copy(void * dst, void const* src, std::size_t len)
        {
            memcpy(dst, src, len);
        }
    };

} //namespace stream

namespace {
    using stream::fast_buffer;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_FAST_BUFFER_HPP__