#include "TestPCH.h"
#include <Bex/bexio/nonblocking_circularbuffer.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
using namespace Bex::bexio;

void sgetn_thread(nonblocking_circularbuffer & stream, char * data, std::size_t size)
{
    char * pos = data;
    while (size)
    {
        std::size_t count = stream.sgetn(pos, size);
        pos += count, size -= count;
    }
}

void sputn_thread(nonblocking_circularbuffer & stream, char const* data, std::size_t size)
{
    char const* pos = data;
    while (size)
    {
        std::size_t count = stream.sputn(pos, size);
        pos += count, size -= count;
    }
}

void get_thread(nonblocking_circularbuffer & stream, char * data, std::size_t size)
{
    char * pos = data;
    while (size)
    {
        std::size_t count = (std::min)(stream.gcount(), size);
        if (!count)
            continue;

        memcpy(pos, stream.gptr(), count);
        stream.gbump(count);
        pos += count, size -= count;
    }
}

void put_thread(nonblocking_circularbuffer & stream, char const* data, std::size_t size)
{
    char const* pos = data;
    while (size)
    {
        std::size_t count = (std::min)(stream.pcount(), size);
        if (!count)
            continue;

        memcpy(stream.pptr(), pos, count);
        stream.pbump(count);
        pos += count, size -= count;
    }
}

void get_mb_thread(nonblocking_circularbuffer & stream, char * data, std::size_t size)
{
    char * pos = data;
    while (size)
    {
        boost::array<boost::asio::const_buffer, 2> mbs;
        std::size_t buf_count = stream.get_buffers(mbs);
        if (!buf_count)
            continue;

        for (std::size_t i = 0; i < buf_count; ++i)
        {
            std::size_t once = (std::min)(boost::asio::detail::buffer_size_helper(mbs[i]), size);
            memcpy(pos, boost::asio::detail::buffer_cast_helper(mbs[i]), once);
            stream.gbump(once);
            pos += once, size -= once;
        }
    }
}

void put_cb_thread(nonblocking_circularbuffer & stream, char const* data, std::size_t size)
{
    char const* pos = data;
    while (size)
    {
        boost::array<boost::asio::mutable_buffer, 2> mbs;
        std::size_t buf_count = stream.put_buffers(mbs);
        if (!buf_count)
            continue;

        for (std::size_t i = 0; i < buf_count; ++i)
        {
            std::size_t once = (std::min)(boost::asio::detail::buffer_size_helper(mbs[i]), size);
            memcpy(boost::asio::detail::buffer_cast_helper(mbs[i]), pos, once);
            stream.pbump(once);
            pos += once, size -= once;
        }
    }
}

BOOST_AUTO_TEST_SUITE(s_bexio)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_bexio_noblocking_circle)
{
    XDump("开始测试 noblocking_circle");

    {
        static const int size = 1024 * 1024;
        char * src = new char[size];
        char * dst = new char[size];

        for (int i = 0; i < size; ++i)
            src[i] = i % 10 + '0';
        memset(dst, 0, size);

        char buf[193] = {};
        nonblocking_circularbuffer stream(buf, sizeof(buf));

        {
            boost::progress_timer bgt;
            //boost::thread th_get(boost::bind(&get_thread, boost::ref(stream), dst, size));
            //boost::thread th_put(boost::bind(&put_thread, boost::ref(stream), src, size));

            //boost::thread th_get(boost::bind(&sgetn_thread, boost::ref(stream), dst, size));
            //boost::thread th_put(boost::bind(&sputn_thread, boost::ref(stream), src, size));

            boost::thread th_get(boost::bind(&get_mb_thread, boost::ref(stream), dst, size));
            boost::thread th_put(boost::bind(&put_cb_thread, boost::ref(stream), src, size));
            th_get.join(), th_put.join();
        }

        for (int i = 0; i < size; ++i)
        {
            if (src[i] != dst[i])
            {
                break;
            }
        }

        BOOST_CHECK((memcmp(src, dst, size) == 0));
    }

    XDump("结束测试 noblocking_circle");
}

BOOST_AUTO_TEST_SUITE_END()