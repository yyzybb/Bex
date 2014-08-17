#include "TestPCH.h"
//#define BEX_SERIALIZATION_POD_EXTEND 
#include <Bex/stream.hpp>
#include <Bex/timer/timer.hpp>

struct Pod_t1
{
    int i_;
};

struct Pod_t2
{
    Pod_t1 tl[12];
};

struct Pod_t3
{
    Pod_t2 tl[12];

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & tl;
    }
};

struct Obj_t1
{
    int i_;
    std::string s_;

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & i_ & s_;
    }
};

struct Obj_t2
{
    Obj_t1 tl[12];

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & tl;
    }
};

struct Obj_t3
{
    Obj_t2 tl[12];

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & tl;
    }
};

BOOST_AUTO_TEST_SUITE(s_stream_suite)

/// ≤‚ ‘serialization–‘ƒ‹
BOOST_AUTO_TEST_CASE(t_serialization_property)
{
    const int buf_size = 1024 * 1024 * 100;
    char* buf = new char[buf_size];

#if defined(_DEBUG) || defined(DEBUG)
    const int tc = 100000;
#else
    const int tc = 10000000;
#endif

    // ‘§»»
    {
        static_streambuf ssb(buf, buf_size);
        binary_oarchive bo(ssb);
        for (int i = 0; i < tc; ++i)
            bo << i;
    }

    /// 1.test int
    {
        static_streambuf ssb(buf, buf_size);
        binary_oarchive bo(ssb);

        // save
        Bex::timer bt;
        for (int i = 0; i < tc; ++i)
            bo << i;
        Dump("data size:" << ssb.size());
        Dump("binary save int cost time:" << bt.elapsed() << "s");
    }

    /// 2.test byte
    {
        static_streambuf ssb(buf, buf_size);
        binary_oarchive bo(ssb);

        // save
        Bex::timer bt;
        for (int i = 0; i < tc; ++i)
            bo << (unsigned char)i;
        Dump("data size:" << ssb.size());
        Dump("binary save unsigned char cost time:" << bt.elapsed() << "s");
    }

    /// 2.test string
    {
        static_streambuf ssb(buf, buf_size);
        binary_oarchive bo(ssb);

        // save
        std::string str = "abcdefghijklmnopqrstuvwxyz@1234567890";
        Bex::timer bt;
        for (int i = 0; i < tc / 10; ++i)
            bo << str;
        Dump("data size:" << ssb.size());
        Dump("binary save std::string cost time:" << bt.elapsed() << "s");
    }

    /// 2.test array optimize
    {
        static_streambuf ssb(buf, buf_size);
        binary_oarchive bo(ssb);

        // save
        Pod_t3 obj;
        Bex::timer bt;
        for (int i = 0; i < tc / 100; ++i)
            bo << obj;
        Dump("data size:" << ssb.size());
        Dump("binary save Pod_t3 cost time:" << bt.elapsed() << "s");
    }

    /// 2.test array optimize
    {
        static_streambuf ssb(buf, buf_size);
        binary_oarchive bo(ssb);
        binary_iarchive bi(ssb);

        // save
        Obj_t3 obj;
        Bex::timer bt;
        for (int i = 0; i < tc / 100; ++i)
            bo << obj;
        Dump("data size:" << ssb.size());
        Dump("binary save Obj_t3 cost time:" << bt.elapsed() << "s");

        // load
        bt.restart();
        for (int i = 0; i < tc / 100; ++i)
            bi >> obj;
        BOOST_CHECK(ssb.size() == 0);
        Dump("binary load Obj_t3 cost time:" << bt.elapsed() << "s");
    }
}

BOOST_AUTO_TEST_SUITE_END()