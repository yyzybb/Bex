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
    const int buf_size = 1024 * 1024 * 400;
    char* buf = new char[buf_size];

#if defined(_DEBUG) || defined(DEBUG)
    const int tc = 100000;
#else
    const int tc = 10000000;
#endif

    // ‘§»»
    {
        fast_buffer ssb(buf, buf_size);
        binary_oarchive<fast_buffer> bo(ssb);
        for (int i = 0; i < tc * 10; ++i)
            bo << i;
        Dump("HOO [" << ssb.gcount() << "]");
    }

    /// 1.test int
    {
        fast_buffer ssb(buf, buf_size);
        binary_oarchive<fast_buffer> bo(ssb);
        binary_iarchive<fast_buffer> bi(ssb);

        int t = tc * 10;

        // save
        Bex::timer bt;
        for (int i = 0; i < t; ++i)
            bo << i;
        Dump("data size:" << ssb.gcount());
        Dump("binary save `int` cost time:" << bt.elapsed() << "s");

        // load
        std::size_t s = ssb.gcount();
        int v = 0;
        bt.restart();
        for (int i = 0; i < t; ++i)
            bi >> v;
        Dump("data size:" << s - ssb.gcount());
        Dump("binary load `int` cost time:" << bt.elapsed() << "s");
    }

    /// 2.test byte
    {
        fast_buffer ssb(buf, buf_size);
        binary_oarchive<fast_buffer> bo(ssb);
        binary_iarchive<fast_buffer> bi(ssb);

        int t = tc * 10;

        // save
        Bex::timer bt;
        for (int i = 0; i < t; ++i)
            bo << (unsigned char)i;
        Dump("data size:" << ssb.gcount());
        Dump("binary save `unsigned char` cost time:" << bt.elapsed() << "s");

        // load
        std::size_t s = ssb.gcount();
        unsigned char v = 0;
        bt.restart();
        for (int i = 0; i < t; ++i)
            bi >> v;
        Dump("data size:" << s - ssb.gcount());
        Dump("binary load `unsigned char` cost time:" << bt.elapsed() << "s");
    }

    /// 3.test string
    {
        fast_buffer ssb(buf, buf_size);
        binary_oarchive<fast_buffer> bo(ssb);
        binary_iarchive<fast_buffer> bi(ssb);

        int t = tc / 10;

        // save
        std::string str = "abcdefghijklmnopqrstuvwxyz@1234567890";
        Bex::timer bt;
        for (int i = 0; i < t; ++i)
            bo << str;
        Dump("data size:" << ssb.gcount());
        Dump("binary save std::string cost time:" << bt.elapsed() << "s");

        //load
        std::size_t s = ssb.gcount();
        std::string v;
        bt.restart();
        for (int i = 0; i < t; ++i)
            bi >> v;
        Dump("data size:" << s - ssb.gcount());
        Dump("binary load std::string cost time:" << bt.elapsed() << "s");
    }

    /// 4.test array optimize
    {
        fast_buffer ssb(buf, buf_size);
        binary_oarchive<fast_buffer> bo(ssb);
        binary_iarchive<fast_buffer> bi(ssb);

        int t = tc / 100;

        // save
        Pod_t3 obj;
        Bex::timer bt;
        for (int i = 0; i < t; ++i)
            bo << obj;
        Dump("data size:" << ssb.gcount());
        Dump("binary save Pod_t3 cost time:" << bt.elapsed() << "s");

        // load
        std::size_t s = ssb.gcount();
        Pod_t3 v;
        bt.restart();
        for (int i = 0; i < t; ++i)
            bi >> v;
        Dump("data size:" << s - ssb.gcount());
        Dump("binary load Pod_t3 cost time:" << bt.elapsed() << "s");
    }

    /// 5.test array optimize
    {
        fast_buffer ssb(buf, buf_size);
        binary_oarchive<fast_buffer> bo(ssb);
        binary_iarchive<fast_buffer> bi(ssb);

        int t = tc / 100;

        // save
        Obj_t3 obj;
        Bex::timer bt;
        for (int i = 0; i < t; ++i)
            bo << obj;
        Dump("data size:" << ssb.gcount());
        Dump("binary save Obj_t3 cost time:" << bt.elapsed() << "s");

        // load
        bt.restart();
        for (int i = 0; i < t; ++i)
            bi >> obj;
        BOOST_CHECK(ssb.gcount() == 0);
        Dump("binary load Obj_t3 cost time:" << bt.elapsed() << "s");
    }
}

BOOST_AUTO_TEST_SUITE_END()