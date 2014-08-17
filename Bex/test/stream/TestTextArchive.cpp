#include "TestPCH.h"
#include <Bex/stream.hpp>

struct TestTextStruct
{
    typedef std::vector<int> V1;
    typedef std::vector<V1> V2;
    typedef std::vector<V2> V3;

    int i;
    double db;
    std::vector<int> vec;
    std::vector<std::string> vec_str;
    int arr[10];
    std::wstring arr_str[3];
    V3 vec3;

    enum {BEX_SS_VERSION = 1,};

    typedef TestTextStruct this_type;

    template <class Container>
    static bool check_container(Container const& lhs, Container const& rhs)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class Container, class Pr>
    static bool check_container_pr(Container const& lhs, Container const& rhs, Pr pred)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
    }

    bool operator==(const this_type& rhs) const
    {
        bool ret = i == rhs.i;
        ret &= db == rhs.db;
        ret &= check_container(vec, rhs.vec);
        ret &= check_container_pr(vec_str, rhs.vec_str, &check_container<std::string>);
        ret &= std::equal(&arr[0], &arr[0] + sizeof(arr) / sizeof(int), &rhs.arr[0]);
        ret &= std::equal(&arr_str[0], &arr_str[0] + sizeof(arr_str) / sizeof(std::wstring)
            , &rhs.arr_str[0], &check_container<std::wstring>);

        typedef boost::function<bool(V1 const&, V1 const&)> PrV1;
        typedef boost::function<bool(V2 const&, V2 const&)> PrV2;
        PrV1 pr_v1 = boost::bind(&check_container<V1>, _1, _2);
        PrV2 pr_v2 = boost::bind(&check_container_pr<V2, PrV1>, _1, _2, pr_v1);
        ret &= check_container_pr(vec3, rhs.vec3, pr_v2);
        return ret;
    }
};

template <class Archive>
void serialize(Archive & ar, TestTextStruct & t, const unsigned int version)
{
    BOOST_CHECK_EQUAL(version, 1);
    ar & t.i;
    ar & t.db;
    ar & t.vec;
    ar & t.vec_str;
    ar & t.arr;
    ar & t.arr_str;
    ar & t.vec3;
}

struct fake_pod_struct
{
    int i;
    double db;
    float xy[12];

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & i & db & xy;
    }
};

struct TestIncConvert
{
    int i;
    short s;
    std::string str;
    char arr_char[10];

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & inc_cvt<Archive>(i);
        ar & inc_cvt(s, &ar);
        ar & a2w_cvt<Archive>(str);
        ar & a2w_cvt<Archive>(arr_char);
    }
};

struct TestRollback
{
    int i;
    int j;

    template <class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & i & j;
    }
};

BOOST_AUTO_TEST_SUITE(s_stream_text_suite)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_stream_text_archive_case)
{
    XDump("开始测试 stream.serialization.text_archive");

    BOOST_STATIC_ASSERT((Bex::serialization::has_serialize<Bex::cn32>::value));

    /// text_oarchive -> std::cout
    {
        text_oarchive bo(std::cout);
        bo << std::string("abcdefghijabcdefghijabcdefghijabcdefghij@\n");
        bo << "abc" << "\n";
    }

#ifdef _DEBUG
    const int len = 3000000;
#else //_DEBUG
    const int len = 10000000;
#endif //_DEBUG

    char buf[4096 * 10] = {};

    /// test static_streambuf
    {
        static_streambuf ssb(buf, 12);
        {
            text_oarchive bo(ssb);
            text_iarchive bi(ssb);

            char data[15] = "12345678901234";
            char check[15] = {};

            BOOST_CHECK(!bo.save(data));
            BOOST_CHECK(ssb.size() == 12);

            BOOST_CHECK(!bi.load(check));
            BOOST_CHECK(ssb.size() == 0);

            BOOST_CHECK(memcmp(data, check, 15) != 0);
            BOOST_CHECK(memcmp(data, check, 12) == 0);

            BOOST_CHECK(!bo.save('x'));
        }
    }

    /// iarchive oarchive
    {
        /// vector
        static_streambuf ssb(buf, sizeof(buf));
        text_oarchive bo(ssb);
        text_iarchive bi(ssb);

        BOOST_CHECK_EQUAL(ssb.size(), 0);

        std::vector<int> x(1), check;

        bo & x;
        bi & check;

        BOOST_CHECK( x.size() == check.size() && std::equal(x.begin(), x.end(), check.begin()));
    }

    {
        ring_streambuf rsb(buf, sizeof(buf));
        text_oarchive bo(rsb);
        text_iarchive bi(rsb);

        BOOST_CHECK_EQUAL(rsb.size(), 0);

        TestTextStruct x, check;
        x.i = 0, x.db = 12.7, x.vec.push_back(1), x.vec.push_back(3), x.vec.push_back(2);

        bo & x;
        //BOOST_CHECK_EQUAL(rsb.size(), 26);
        bi & check;
        BOOST_CHECK_EQUAL(rsb.size(), 0);

        BOOST_CHECK(x == check);
    }

    /// io_archive class
    {
        ring_streambuf rsb(buf, sizeof(buf));
        text_archive bio(rsb);

        BOOST_CHECK_EQUAL(rsb.size(), 0);

        TestTextStruct x, check;
        x.i = 0, x.db = 12.7, x.vec.push_back(1), x.vec.push_back(3), x.vec.push_back(2);
        std::vector<std::string> tmp_vec(3, std::string("asdlkjfweo"));
        x.vec_str.swap(tmp_vec);
        FOR(10)
            x.arr[i] = i;
        FOR(3)
            x.arr_str[i] = L"2j3oi4uo";

        x.vec3.resize(5);
        FOR(5)
        {
            TestTextStruct::V2 & v2 = x.vec3[i];
            v2.resize(10);
            UFOR(10)
            {
                TestTextStruct::V1 & v1 = v2[ui];
                v1.push_back(1);
                v1.push_back(2);
                v1.push_back(i + ui);
            }
        }

        bio << x;
        //BOOST_CHECK_EQUAL(rsb.size(), 26);
        bio >> check;
        BOOST_CHECK_EQUAL(rsb.size(), 0);

        BOOST_CHECK(x == check);

        std::vector<bool> vb, vb_check;
        vb.reserve(100);
        FOR(1000)
            vb.push_back(true);
        //vb.clear();
        //vb.push_back(true);
        //vb.push_back(false);
        //vb.push_back(true);
        //vb.size();

        bio << vb;
        bio >> vb_check;

        BOOST_CHECK_EQUAL(vb.size(), vb_check.size());
        UFOR(vb.size())
        {
            BOOST_CHECK_EQUAL(vb[ui], vb_check[ui]);
        }
    }

    /// text_load text_save
    {
        TestTextStruct x, check;
        x.i = 0, x.db = 12.7, x.vec.push_back(1), x.vec.push_back(3), x.vec.push_back(2);
        std::vector<std::string> tmp_vec(3, std::string("asdlkjfweo"));
        x.vec_str.swap(tmp_vec);
        FOR(10)
            x.arr[i] = i;
        FOR(3)
            x.arr_str[i] = L"2j3oi4uo";

        x.vec3.resize(5);
        FOR(5)
        {
            TestTextStruct::V2 & v2 = x.vec3[i];
            v2.resize(10);
            UFOR(10)
            {
                TestTextStruct::V1 & v1 = v2[ui];
                v1.push_back(1);
                v1.push_back(2);
                v1.push_back(i + ui);
            }
        }

        std::size_t save_len = text_save(x, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        std::size_t load_len = text_load(check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        BOOST_CHECK_EQUAL(save_len, load_len);
        BOOST_CHECK(x == check);

        std::vector<bool> vb, vb_check;
        vb.reserve(100);
        FOR(1000)
            vb.push_back(true);
        vb.clear();
        vb.push_back(true);
        vb.push_back(false);
        vb.push_back(true);
        vb.size();

        save_len = text_save(vb, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        load_len = text_load(vb_check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        BOOST_CHECK_EQUAL(save_len, load_len);

        BOOST_CHECK_EQUAL(vb.size(), vb_check.size());
        UFOR(vb.size())
        {
            BOOST_CHECK_EQUAL(vb[ui], vb_check[ui]);
        }
    }

    {
        fake_pod_struct obj[10], check[10];
        BOOST_CHECK( !Bex::serialization::is_binary_trivial<fake_pod_struct>::value );

        std::size_t save_len = text_save(obj, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        std::size_t load_len = text_load(check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        BOOST_CHECK_EQUAL(save_len, load_len);

        BOOST_CHECK(memcmp(obj, check, sizeof(obj)) == 0);
    }

    XDump("结束测试 stream.serialization.text_archive");
}

/// 性能测试
BOOST_AUTO_TEST_CASE(t_stream_text_archive_property_case)
{
    XDump("开始测试 stream.serialization.text_archive性能");

#ifdef _DEBUG
    const int tc = 10000;
#else  //_DEBUG
    const int tc = 10000 * 10;
#endif //_DEBUG

    {
        const int len = 160 * tc;
        char *buf = new char[len];
        static_streambuf ssb(buf, len);
        text_archive bio(ssb);
        TestTextStruct obj;

        //DumpX(detail::has_version<TestStreamStruct>::value);

        {
            std::cout << tc << " times save, cost time: ";
            boost::progress_timer pt;
            FOR(tc)
                bio << obj;
        }

        {
            std::cout << tc << " times load, cost time: ";
            boost::progress_timer pt;
            FOR(tc)
                bio >> obj;
        }

        BOOST_CHECK_EQUAL(ssb.size(), 0);

        ssb.reset();

        char pod_obj[59];

        {
            std::cout << tc << " times save pod_obj, cost time: ";
            boost::progress_timer pt;
            FOR(tc)
                bio << pod_obj;
        }

        {
            std::cout << tc << " times load pod_obj, cost time: ";
            boost::progress_timer pt;
            FOR(tc)
                bio >> pod_obj;
        }

        BOOST_CHECK_EQUAL(ssb.size(), 0);

        ssb.reset();

        {
            char src[60] = {};
            std::cout << tc << " times streambuf::putn, cost time: ";
            boost::progress_timer pt;
            FOR(tc)
                ssb.sputn(src, sizeof(src));
        }

        {
            char src[60] = {};
            std::cout << tc << " times streambuf::getn, cost time: ";
            boost::progress_timer pt;
            FOR(tc)
                ssb.sgetn(src, sizeof(src));
        }

        BOOST_CHECK_EQUAL(ssb.size(), 0);
    }

    XDump("结束测试 stream.serialization.text_archive性能");
}

BOOST_AUTO_TEST_SUITE_END()