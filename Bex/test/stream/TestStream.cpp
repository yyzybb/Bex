#include "TestPCH.h"
//#define BEX_SERIALIZATION_POD_EXTEND 
#include <Bex/stream.hpp>
#include <Bex/locale/charset_cvt.h>

struct TestStreamStruct
{
    typedef std::vector<int> V1;
    typedef std::vector<V1> V2;
    typedef std::vector<V2> V3;

    typedef std::list<int>                  List;
    typedef std::deque<std::string>         Deque;
    typedef std::set<std::string>           Set;
    typedef std::multiset<int>              MultiSet;
    typedef std::map<std::string, V2>       Map;
    typedef std::multimap<std::string, V2>  MultiMap;

#if defined(_MSC_VER)
    typedef stdext::hash_map<int, std::string> HashMap;
#endif //defined(_MSC_VER)

#if defined(BOOST_HAS_TR1_UNORDERED_MAP)
    typedef std::unordered_map<int, std::string>        UnorderedMap;
    typedef std::unordered_multimap<int, std::string>   UnorderedMultiMap;
#endif //defined(BOOST_HAS_TR1_UNORDERED_MAP)

#if defined(BOOST_HAS_TR1_UNORDERED_SET)
    typedef std::unordered_set<std::string>             UnorderedSet;
    typedef std::unordered_multiset<std::string>        UnorderedMultiSet;
#endif //defined(BOOST_HAS_TR1_UNORDERED_SET)

#if defined(BEX_SUPPORT_CXX11)
    typedef std::array<std::string, 7>                  StdArray;
#endif //defined(BEX_SUPPORT_CXX11)

    typedef boost::array<int, 3> BoostArray;
    typedef boost::bimap<int, int> BoostBimap;

    int i;
    double db;
    std::vector<int> vec;
    std::vector<std::string> vec_str;
    int arr[10];
    std::wstring arr_str[3];
    V3 vec3;

    List list;
    Deque deque;
    Set set;
    MultiSet mset;
    Map map;
    MultiMap mmap;
#if defined(_MSC_VER)
    HashMap hmap;
#endif //defined(_MSC_VER)
#if defined(BOOST_HAS_TR1_UNORDERED_MAP)
    UnorderedMap umap;
    UnorderedMultiMap ummap;
#endif //defined(BOOST_HAS_TR1_UNORDERED_MAP)
#if defined(BOOST_HAS_TR1_UNORDERED_SET)
    UnorderedSet uset;
    UnorderedMultiSet umset;
#endif //defined(BOOST_HAS_TR1_UNORDERED_SET)
#if defined(BEX_SUPPORT_CXX11)
    StdArray sarray;
#endif //defined(BEX_SUPPORT_CXX11)
    BoostArray barray;
    BoostBimap bbimap;

    typedef TestStreamStruct this_type;

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

    template <class HashContainer, class Pr>
    static bool check_hashmap_container(HashContainer const& lhs, HashContainer const& rhs, Pr pred)
    {
        if (lhs.size() != rhs.size()) return false;
        typedef typename HashContainer::const_iterator iterator;
        for (iterator it1 = lhs.begin(); it1 != lhs.end(); ++it1)
        {
            iterator it2 = rhs.find(it1->first);
            if (it2 == rhs.end() || !pred(it2->second, it1->second))
                return false;
        }
        return true;
    }

    template <class HashContainer, class Pr>
    static bool check_hashmultimap_container(HashContainer const& lhs, HashContainer const& rhs, Pr pred)
    {
        if (lhs.size() != rhs.size()) return false;
        typedef typename HashContainer::const_iterator iterator;
        for (iterator it1 = lhs.begin(); it1 != lhs.end(); ++it1)
        {
            if (lhs.count(it1->first) != rhs.count(it1->first)) return false;

            BOOST_AUTO(r, rhs.equal_range(it1->first));
            if (r.first == r.second) return false;

            iterator it2 = r.first;
            for (; it2 != r.second; ++it2)
                if (pred(it2->second, it1->second))
                    break;

            if (it2 == r.second)
                return false;
        }
        return true;
    }

    template <class HashContainer>
    static bool check_hashset_container(HashContainer const& lhs, HashContainer const& rhs)
    {
        if (lhs.size() != rhs.size()) return false;
        typedef typename HashContainer::const_iterator iterator;
        for (iterator it1 = lhs.begin(); it1 != lhs.end(); ++it1)
        {
            iterator it2 = rhs.find(*it1);
            if (it2 == rhs.end())
                return false;
        }
        return true;
    }

    template <class HashContainer>
    static bool check_hashmultiset_container(HashContainer const& lhs, HashContainer const& rhs)
    {
        if (lhs.size() != rhs.size()) return false;
        typedef typename HashContainer::const_iterator iterator;
        for (iterator it1 = lhs.begin(); it1 != lhs.end(); ++it1)
        {
            if (lhs.count(*it1) != rhs.count(*it1)) return false;
        }
        return true;
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
        ret &= check_container(list, rhs.list);
        ret &= check_container(deque, rhs.deque);
        ret &= check_container(set, rhs.set);
        ret &= check_container(mset, rhs.mset);
        ret &= check_hashmap_container(map, rhs.map, pr_v2);
        ret &= check_hashmap_container(mmap, rhs.mmap, pr_v2);
#if defined(_MSC_VER)
        ret &= check_hashmap_container(hmap, rhs.hmap, Bex::equal_to<>() );
#endif //defined(_MSC_VER)
#if defined(BOOST_HAS_TR1_UNORDERED_MAP)
        ret &= check_hashmap_container(umap, rhs.umap, Bex::equal_to<>() );
        ret &= check_hashmultimap_container(ummap, rhs.ummap, Bex::equal_to<>() );
#endif //defined(BOOST_HAS_TR1_UNORDERED_MAP)
#if defined(BOOST_HAS_TR1_UNORDERED_SET)
        ret &= check_hashset_container(uset, rhs.uset);
        ret &= check_hashmultiset_container(umset, rhs.umset);
#endif //defined(BOOST_HAS_TR1_UNORDERED_SET)
#if defined(BEX_SUPPORT_CXX11)
        ret &= check_container(sarray, rhs.sarray);
#endif //defined(BEX_SUPPORT_CXX11)
        ret &= check_container(barray, rhs.barray);
        ret &= check_hashmap_container(bbimap.left, rhs.bbimap.left, Bex::equal_to<>() );
        return ret;
    }

    void init_ext_containers()
    {
#if defined(BEX_SUPPORT_CXX11)
        list = List{1, 2, 3, 5, 7};
        deque = Deque{"abc", "xxd", "fffff", ""};
        set = Set{"abc", "xxd", "fffff", ""};
        mset = MultiSet{1, 1, 1, 1, 2, 3, 3, 3, 66, 66, 7};
        map = Map{ {"a", {{1, 2, 1}, {2, 3, 4}}}, {"b", {{1, 2, 1}, {2, 3, 4}}} };
        mmap = MultiMap{ {"a", {{1, 2, 1}, {2, 3, 4}}}, {"a", {{1, 2, 1}, {2, 3, 4}}}, {"c", {{1, 2, 1}, {2, 3, 4}}} };
#if defined(_MSC_VER)
        hmap = HashMap{{1, "abc"}, {2, "ddc"}, {1024, ""}};
#endif //defined(_MSC_VER)
#if defined(BOOST_HAS_TR1_UNORDERED_MAP)
        umap = UnorderedMap{{1, "abc"}, {2, "ddc"}, {1024, ""}};;
        ummap = UnorderedMultiMap{{1, "abc"}, {1, "ddc"}, {1024, ""}};;
#endif //defined(BOOST_HAS_TR1_UNORDERED_MAP)
#if defined(BOOST_HAS_TR1_UNORDERED_SET)
        uset = UnorderedSet{"abc", "xxd", "fffff", ""};
        umset = UnorderedMultiSet{"abc", "xxd", "fffff", "", "", "xxd"};
#endif //defined(BOOST_HAS_TR1_UNORDERED_SET)
        sarray = StdArray{"a", "ab", "abc", "abcd", "abcde", ""};
        barray = BoostArray{1, 2, 3};
        bbimap.left.insert(std::make_pair(1, 2));
        bbimap.left.insert(std::make_pair(3, 4));
        bbimap.left.insert(std::make_pair(5, 6));
#endif //defined(BEX_SUPPORT_CXX11)
    }

    unsigned int serialize_version() const
    {
        return 1;
    }
};

template <class Archive>
void serialize(Archive & ar, TestStreamStruct & t, const unsigned int version)
{
    BOOST_CHECK_EQUAL(version, 1);
    ar & t.i & t.db & t.vec;
    ar & t.vec_str;
    ar & t.arr;
    ar & t.arr_str;
    ar & t.vec3;

    ar & t.list;
    ar & t.deque;
    ar & t.set;
    ar & t.mset;
    ar & t.map;
    ar & t.mmap;
#if defined(_MSC_VER)
    ar & t.hmap;
#endif //defined(_MSC_VER)
#if defined(BOOST_HAS_TR1_UNORDERED_MAP)
    ar & t.umap;
    ar & t.ummap;
#endif //defined(BOOST_HAS_TR1_UNORDERED_MAP)
#if defined(BOOST_HAS_TR1_UNORDERED_SET)
    ar & t.uset;
    ar & t.umset;
#endif //defined(BOOST_HAS_TR1_UNORDERED_SET)
#if defined(BEX_SUPPORT_CXX11)
    ar & t.sarray;
#endif //defined(BEX_SUPPORT_CXX11)
    ar & t.barray;
    ar & t.bbimap;
}

struct fake_pod_struct
{
    int i;
    double db;
    float xy[12];
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

struct pod_t
{
    char buf[59];
};

struct not_pod
{
    not_pod() {}
};

struct static_version
{
    static unsigned int serialize_version()
    {
        return 2;
    }
};

struct friend_version
{
    friend unsigned int serialize_version(friend_version*)
    {
        return 3;
    }
};

struct external_version
{
};
unsigned int serialize_version(external_version*)
{
    return 4;
}

template <class R>
static void put_n(R & rs, char const* buffer, std::size_t size)
{
    Dump("start put " << __FUNCTION__ << "<" << typeid(R).name() << ">");
    while (size)
    {
        std::size_t len = (std::size_t)rs.sputn(buffer, size);
        buffer += len;
        size -= len;
    }
    Dump("end put " << __FUNCTION__ << "<" << typeid(R).name() << ">");
}

template <class R>
static void get_n(R & rs, char * buffer, std::size_t size)
{
    Dump("start get " << __FUNCTION__ << "<" << typeid(R).name() << ">");
    while (size)
    {
        std::size_t len = (std::size_t)rs.sgetn(buffer, size);
        buffer += len;
        size -= len;
    }
    Dump("end get " << __FUNCTION__ << "<" << typeid(R).name() << ">");
}

template <class RBuffer>
static void put_rb(RBuffer & rb, char const* buffer, std::size_t size)
{
    Dump("start put " << __FUNCTION__ << "<" << typeid(RBuffer).name() << ">");
    while (size)
    {
        std::size_t len = (std::min)(rb.pcount(), size);
        if (!len) continue;
        memcpy(rb.pptr(), buffer, len);
        rb.pbump(len);
        buffer += len;
        size -= len;
    }
    Dump("end put " << __FUNCTION__ << "<" << typeid(RBuffer).name() << ">");
}

template <class RBuffer>
static void get_rb(RBuffer & rb, char * buffer, std::size_t size)
{
    Dump("start get " << __FUNCTION__ << "<" << typeid(RBuffer).name() << ">");
    while (size)
    {
        std::size_t len = (std::min)(rb.gcount(), size);
        if (!len) continue;
        memcpy(buffer, rb.gptr(), len);
        rb.gbump(len);
        buffer += len;
        size -= len;
    }
    Dump("end get " << __FUNCTION__ << "<" << typeid(RBuffer).name() << ">");
}

template <typename T>
void check_function(T &)
{
    //DumpX(typeid(T).name());
    BOOST_CHECK(typeid(typename remove_all<T>::type) == typeid(int));
}

template <typename T>
struct check_class
{
    BOOST_STATIC_ASSERT(boost::is_same<typename remove_all<T>::type, int>::value);
    //BOOST_STATIC_ASSERT(false);
};

template <typename T>
inline void assert_convert(T const&
    , bool is_convert, bool is_load, bool is_save, bool is_unkown)
{
    typedef convert_traits<T> traits;
    bool _is_convert = traits::is_convert;
    bool _is_load = traits::is_load;
    bool _is_save = traits::is_save;
    bool _is_unkown = traits::is_unkown;
    BOOST_CHECK_EQUAL(_is_convert, is_convert);
    BOOST_CHECK_EQUAL(_is_load, is_load);
    BOOST_CHECK_EQUAL(_is_save, is_save);
    BOOST_CHECK_EQUAL(_is_unkown, is_unkown);
}

BOOST_AUTO_TEST_SUITE(s_stream_suite)

/// 测试convert
BOOST_AUTO_TEST_CASE(t_stream_convert_case)
{
    XDump("开始测试 stream.convert");

    std::string str;
    assert_convert(a2w_cvt<binary_iarchive>(str), true, true, false, false);
    assert_convert(a2w_cvt<binary_oarchive>(str), true, false, true, false);
    assert_convert(str, false, false, false, true);

    char buf[4096] = {};

    {
        TestIncConvert obj, check;
        obj.i = 2;
        obj.s = 3;
        obj.str = conv::u82a("我们");
        strcpy(obj.arr_char, conv::u82a("幼稚").c_str());
        BOOST_CHECK(binary_save(obj, buf, sizeof(buf)) > 0);

        BOOST_CHECK(*(int*)buf == 3);
        BOOST_CHECK(*(short*)(&buf[0] + sizeof(int)) = 4);

        BOOST_CHECK(binary_load(check, buf, sizeof(buf)) > 0);
        BOOST_CHECK_EQUAL(obj.i, check.i);
        BOOST_CHECK_EQUAL(obj.s, check.s);
        BOOST_CHECK(obj.str == check.str);
        BOOST_CHECK(strcmp(obj.arr_char, check.arr_char) == 0);
    }

    XDump("结束测试 stream.convert");
}

/// 测试remove_all
BOOST_AUTO_TEST_CASE(t_stream_utility_case)
{
    int i; check_function(i); check_class<int> cc_int;
    int const ci = 0; check_function(ci); check_class<int const> cc_int_c;
    int volatile vi = 0; check_function(vi); check_class<int volatile> cc_int_v;
    int const volatile cvi = 0; check_function(cvi); check_class<int const volatile> cc_int_cv;

    int & ref_i = i; check_function(ref_i); check_class<int&> cc_int_r;
    int const& ref_ci = ci; check_function(ref_ci); check_class<int const&> cc_int_cr;
    int volatile& ref_vi = i; check_function(ref_vi); check_class<int volatile&> cc_int_vr;
    int const volatile& ref_cvi = i; check_function(ref_cvi); check_class<int const volatile&> cc_int_cvr;

    int * pi; check_function(pi); check_class<int*> cc_int_p;
    int * const pc_i = 0; check_function(pc_i); check_class<int * const> cc_int_pc;
    int const * p_ci = 0; check_function(p_ci); check_class<int const *> cc_int_cp;
    int const * const pc_ci = 0; check_function(pc_ci); check_class<int const* const> cc_int_cpc;
    int * volatile pv_i = 0; check_function(pv_i); check_class<int * volatile> cc_int_pv;
    int volatile * p_vi = 0; check_function(p_vi); check_class<int volatile *> cc_int_vp;
    int volatile * volatile pv_vi = 0; check_function(pv_vi); check_class<int volatile * volatile> cc_int_vpv;
    int * volatile const pcv_i = 0; check_function(pcv_i); check_class<int * volatile const> cc_int_pvc;
    int volatile const * p_cvi = 0; check_function(p_cvi); check_class<int volatile const*> cc_int_vcp;
    int volatile const * volatile const pcv_cvi = 0; check_function(pcv_cvi); check_class<int volatile const * volatile const> cc_int_cvpcv;

    int arr[5]; check_function(arr); check_class<int[5]> cc_int_a5;
    int const carr[5] = {}; check_function(carr); check_class<int const[5]> cc_int_ca5;

    int *& ref_pi = pi; check_function(ref_pi); check_class<int *&> cc_int_pr;
}

/// 测试static_streambuf
BOOST_AUTO_TEST_CASE(t_static_streambuf)
{
    XDump("开始测试 static_streambuf");

    const int buf_size = 12;
    char buf[buf_size] = {}, data[buf_size];
    FOR(buf_size)
        data[i] = i;

    static_streambuf ssb(buf, sizeof(buf));
    BOOST_CHECK( ssb.sputn(data, 8) == 8 );
    BOOST_CHECK( ssb.size() == 8 );
    BOOST_CHECK( ssb.sgetn(data, 4) == 4 );
    BOOST_CHECK( ssb.size() == 4 );

    BOOST_CHECK( ssb.pubseekoff(-1, std::ios_base::beg, std::ios_base::out) == std::_BADOFF );
    BOOST_CHECK( ssb.pubseekoff(2, std::ios_base::beg, std::ios_base::out) == std::_BADOFF );
    BOOST_CHECK( ssb.pubseekoff(3, std::ios_base::beg, std::ios_base::out) == std::_BADOFF );
    BOOST_CHECK( ssb.pubseekoff(-1, std::ios_base::end, std::ios_base::in) == std::_BADOFF );


    BOOST_CHECK( ssb.pubseekoff(-1, std::ios_base::cur, std::ios_base::in) == (std::streamoff)3 );
    BOOST_CHECK( ssb.size() == 5 );

    BOOST_CHECK( ssb.pubseekoff(-1, std::ios_base::cur, std::ios_base::out) == (std::streamoff)7 );
    BOOST_CHECK( ssb.size() == 4 );

    BOOST_CHECK( ssb.pubseekoff(0, std::ios_base::cur, std::ios_base::in) == (std::streamoff)3 );
    BOOST_CHECK( ssb.size() == 4 );

    BOOST_CHECK( ssb.pubseekoff(0, std::ios_base::beg, std::ios_base::in) == (std::streamoff)0 );
    BOOST_CHECK( ssb.size() == 7 );

    BOOST_CHECK( ssb.pubseekoff(2, std::ios_base::beg, std::ios_base::in) == (std::streamoff)2 );
    BOOST_CHECK( ssb.size() == 5 );

    BOOST_CHECK( ssb.pubseekoff(0, std::ios_base::beg, std::ios_base::out) == std::_BADOFF );
    
    BOOST_CHECK( ssb.pubseekoff(2, std::ios_base::beg, std::ios_base::out) == (std::streamoff)2 );
    BOOST_CHECK( ssb.size() == 0 );

    BOOST_CHECK( ssb.pubseekoff(1, std::ios_base::cur, std::ios_base::out) == (std::streamoff)3 );
    BOOST_CHECK( ssb.size() == 1 );

    BOOST_CHECK( ssb.pubseekoff(7, std::ios_base::cur, std::ios_base::out | std::ios_base::in) == std::_BADOFF );
    BOOST_CHECK( ssb.size() == 1 );

    BOOST_CHECK( ssb.pubseekoff(7, std::ios_base::beg, std::ios_base::out | std::ios_base::in) == (std::streamoff)7 );
    BOOST_CHECK( ssb.size() == 0 );

    XDump("结束测试 static_streambuf");
}

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_stream_case)
{
    XDump("开始测试 stream");
    
    /// binary_oarchive -> std::cout
    {
        binary_oarchive bo(std::cout);
        bo << std::string("abcdefghijabcdefghijabcdefghijabcdefghij@\n");
        bo << "abc" << "\n";
    }

#ifdef _DEBUG
    const int len = 3000000;
#else //_DEBUG
    const int len = 100000000;
#endif //_DEBUG

    char buf[4096 * 10] = {};

    /// test static_streambuf
    {
        static_streambuf ssb(buf, 12);
        {
            binary_oarchive bo(ssb);
            binary_iarchive bi(ssb);

            char data[15] = "12345678901234";
            char check[15] = {};

            BOOST_CHECK(!bo.save(data, sizeof(data)));
            BOOST_CHECK(ssb.size() == 12);

            BOOST_CHECK(!bi.load(check, sizeof(check)));
            BOOST_CHECK(ssb.size() == 0);

            BOOST_CHECK(memcmp(data, check, 15) != 0);
            BOOST_CHECK(memcmp(data, check, 12) == 0);

            BOOST_CHECK(!bo.save('x'));
        }

        {
            ssb.reset();
            binary_oarchive bo(ssb, amb_rollback);
            binary_iarchive bi(ssb, amb_rollback);

            char data[15] = "12345678901234";
            char check[15] = {};

            BOOST_CHECK(!bo.save(data, sizeof(data)));
            BOOST_CHECK(ssb.size() == 0);

            BOOST_CHECK(bo.save(data, 12));
            BOOST_CHECK(ssb.size() == 12);

            BOOST_CHECK(!bi.load(check, sizeof(check)));
            BOOST_CHECK(ssb.size() == 12);

            BOOST_CHECK(memcmp(data, check, 15) != 0);
            BOOST_CHECK(memcmp(data, check, 12) == 0);

            BOOST_CHECK(!bo.save('x'));
        }

        {
            TestRollback obj;

            static_streambuf rb_ssb(buf, 7);
            binary_archive bio(rb_ssb, amb_rollback);

            BOOST_CHECK(!bio.save(obj));
            BOOST_CHECK(rb_ssb.size() == 0);

            char data[7] = {};
            BOOST_CHECK(bio.save(data));
            BOOST_CHECK(rb_ssb.size() == 7);

            BOOST_CHECK(!bio.load(obj));
            BOOST_CHECK(rb_ssb.size() == 7);
        }
    }

    /// test ring_streambuf
    {
        /// single thread
        {
            ring_streambuf rsb(buf, 13);
            binary_oarchive bo(rsb);
            binary_iarchive bi(rsb);

            char data[15] = "12345678901234";
            char check[15] = {};

            BOOST_CHECK(!bo.save(data, sizeof(data)));
            BOOST_CHECK(!bi.load(check, sizeof(check)));

            BOOST_CHECK(memcmp(data, check, 15) != 0);
            BOOST_CHECK(memcmp(data, check, 12) == 0);

            BOOST_CHECK(!bo.save(data, sizeof(data)));
            BOOST_CHECK(!bi.load(check, sizeof(check)));

            BOOST_CHECK(memcmp(data, check, 15) != 0);
            BOOST_CHECK(memcmp(data, check, 12) == 0);
        }

        /// mutli thread
        {
            ring_streambuf rsb(buf, 128);
            char *data = new char[len], *check = new char[len];
            FOR(len)
                data[i] = (i % 10) + '0';

            boost::progress_timer pt;
            boost::thread_group tg;
            tg.create_thread(boost::bind(&put_n<ring_streambuf>, boost::ref(rsb), &data[0], len));
            tg.create_thread(boost::bind(&get_n<ring_streambuf>, boost::ref(rsb), &check[0], len));
            tg.join_all();

            BOOST_ASSERT(memcmp(data, check, len) == 0);

            delete[] data;
            delete[] check;
        }
    }

    /// test ring_buf
    {
        /// mutli thread
        {
            ring_buf rb(buf, 128);
            char *data = new char[len], *check = new char[len];
            FOR(len)
                data[i] = (i % 10) + '0';

            boost::progress_timer pt;
            boost::thread_group tg;
            tg.create_thread(boost::bind(&put_rb<ring_buf>, boost::ref(rb), &data[0], len));
            tg.create_thread(boost::bind(&get_rb<ring_buf>, boost::ref(rb), &check[0], len));
            tg.join_all();

            BOOST_ASSERT(memcmp(data, check, len) == 0);

            delete[] data;
            delete[] check;
        }

        /// mutli thread
        {
            ring_buf rb(buf, 128);
            char *data = new char[len], *check = new char[len];
            FOR(len)
                data[i] = (i % 10) + '0';

            boost::progress_timer pt;
            boost::thread_group tg;
            tg.create_thread(boost::bind(&put_rb<ring_buf>, boost::ref(rb), &data[0], len));
            tg.create_thread(boost::bind(&get_n<ring_buf>, boost::ref(rb), &check[0], len));
            tg.join_all();

            BOOST_ASSERT(memcmp(data, check, len) == 0);

            delete[] data;
            delete[] check;
        }

        /// mutli thread
        {
            ring_buf rb(buf, 128);
            char *data = new char[len], *check = new char[len];
            FOR(len)
                data[i] = (i % 10) + '0';

            boost::progress_timer pt;
            boost::thread_group tg;
            tg.create_thread(boost::bind(&put_n<ring_buf>, boost::ref(rb), &data[0], len));
            tg.create_thread(boost::bind(&get_n<ring_buf>, boost::ref(rb), &check[0], len));
            tg.join_all();

            BOOST_ASSERT(memcmp(data, check, len) == 0);

            delete[] data;
            delete[] check;
        }

        /// mutli thread
        {
            ring_buf rb(buf, 128);
            char *data = new char[len], *check = new char[len];
            FOR(len)
                data[i] = (i % 10) + '0';

            boost::progress_timer pt;
            boost::thread_group tg;
            tg.create_thread(boost::bind(&put_n<ring_buf>, boost::ref(rb), &data[0], len));
            tg.create_thread(boost::bind(&get_rb<ring_buf>, boost::ref(rb), &check[0], len));
            tg.join_all();

            BOOST_ASSERT(memcmp(data, check, len) == 0);

            delete[] data;
            delete[] check;
        }

        /// abort!
        //{
        //    ring_buf rb(buf, 128);
        //    char *data = new char[len], *check = new char[len];
        //    FOR(len)
        //        data[i] = (i % 10) + '0';

        //    boost::progress_timer pt;
        //    boost::thread_group tg;
        //    tg.create_thread(boost::bind(&put_n<ring_buf>, boost::ref(rb), &data[0], len));
        //    tg.create_thread(boost::bind(&put_n<ring_buf>, boost::ref(rb), &data[0], len));
        //    tg.create_thread(boost::bind(&get_rb, boost::ref(rb), &check[0], len));
        //    tg.join_all();

        //    BOOST_ASSERT(memcmp(data, check, len) == 0);

        //    delete[] data;
        //    delete[] check;
        //}
    }

    /// test mutli_ringbuf
    {
        std::size_t elem_capacity = 1024;
        long min_count = 100;
        long max_count_list[] = {200, (std::numeric_limits<long>::max)()};

        for (long i = 0; i < sizeof(max_count_list) / sizeof(long); ++i)
        {
            long max_count = max_count_list[i];

            {
                multi_ringbuf rb(elem_capacity, min_count, max_count);
                char *data = new char[len], *check = new char[len];
                FOR(len)
                    data[i] = (i % 10) + '0';

                boost::progress_timer pt;
                boost::thread_group tg;
                tg.create_thread(boost::bind(&put_n<multi_ringbuf>, boost::ref(rb), &data[0], len));
                tg.create_thread(boost::bind(&get_n<multi_ringbuf>, boost::ref(rb), &check[0], len));
                tg.join_all();

                BOOST_ASSERT(memcmp(data, check, len) == 0);

                delete[] data;
                delete[] check;
            }

            {
                multi_ringbuf rb(elem_capacity, min_count, max_count);
                char *data = new char[len], *check = new char[len];
                FOR(len)
                    data[i] = (i % 10) + '0';

                boost::progress_timer pt;
                boost::thread_group tg;
                tg.create_thread(boost::bind(&put_rb<multi_ringbuf>, boost::ref(rb), &data[0], len));
                tg.create_thread(boost::bind(&get_rb<multi_ringbuf>, boost::ref(rb), &check[0], len));
                tg.join_all();

                BOOST_ASSERT(memcmp(data, check, len) == 0);

                delete[] data;
                delete[] check;
            }

            {
                multi_ringbuf rb(elem_capacity, min_count, max_count);
                char *data = new char[len], *check = new char[len];
                FOR(len)
                    data[i] = (i % 10) + '0';

                boost::progress_timer pt;
                boost::thread_group tg;
                tg.create_thread(boost::bind(&put_n<multi_ringbuf>, boost::ref(rb), &data[0], len));
                tg.create_thread(boost::bind(&get_rb<multi_ringbuf>, boost::ref(rb), &check[0], len));
                tg.join_all();

                BOOST_ASSERT(memcmp(data, check, len) == 0);

                delete[] data;
                delete[] check;
            }

            {
                multi_ringbuf rb(elem_capacity, min_count, max_count);
                char *data = new char[len], *check = new char[len];
                FOR(len)
                    data[i] = (i % 10) + '0';

                boost::progress_timer pt;
                boost::thread_group tg;
                tg.create_thread(boost::bind(&put_rb<multi_ringbuf>, boost::ref(rb), &data[0], len));
                tg.create_thread(boost::bind(&get_n<multi_ringbuf>, boost::ref(rb), &check[0], len));
                tg.join_all();

                BOOST_ASSERT(memcmp(data, check, len) == 0);

                delete[] data;
                delete[] check;
            }
        }
    }

    /// iarchive oarchive
    {
        ring_streambuf rsb(buf, sizeof(buf));
        binary_oarchive bo(rsb);
        binary_iarchive bi(rsb);

        BOOST_CHECK_EQUAL(rsb.size(), 0);

        TestStreamStruct x, check;
        x.i = 0, x.db = 12.7;
        x.vec.push_back(1), x.vec.push_back(3), x.vec.push_back(2);
        for (int i = 0; i < 1023; ++i)
            x.vec.push_back(i);
        x.init_ext_containers();

        bo & x;
        //BOOST_CHECK_EQUAL(rsb.size(), 26);
        bi & check;
        BOOST_CHECK_EQUAL(rsb.size(), 0);

        BOOST_CHECK(x == check);
    }

    /// io_archive class
    {
        ring_streambuf rsb(buf, sizeof(buf));
        binary_archive bio(rsb);

        BOOST_CHECK_EQUAL(rsb.size(), 0);

        TestStreamStruct x, check;
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
            TestStreamStruct::V2 & v2 = x.vec3[i];
            v2.resize(10);
            UFOR(10)
            {
                TestStreamStruct::V1 & v1 = v2[ui];
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

    /// binary_load binary_save
    {
        TestStreamStruct x, check;
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
            TestStreamStruct::V2 & v2 = x.vec3[i];
            v2.resize(10);
            UFOR(10)
            {
                TestStreamStruct::V1 & v1 = v2[ui];
                v1.push_back(1);
                v1.push_back(2);
                v1.push_back(i + ui);
            }
        }

        std::size_t save_len = binary_save(x, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        std::size_t load_len = binary_load(check, buf, sizeof(buf));
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

        save_len = binary_save(vb, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        load_len = binary_load(vb_check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        BOOST_CHECK_EQUAL(save_len, load_len);

        BOOST_CHECK_EQUAL(vb.size(), vb_check.size());
        UFOR(vb.size())
        {
            BOOST_CHECK_EQUAL(vb[ui], vb_check[ui]);
        }
    }

    // vector<bool>
    {
        std::vector<bool> bvector(65535, true), check;

        std::size_t save_len = binary_save(bvector, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        std::size_t load_len = binary_load(check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        bool ok = (bvector.size() == check.size());
        for (std::size_t i = 0; i < bvector.size() && ok; ++i)
            if (bvector[i] != check[i])
                ok = false;
        
        BOOST_CHECK(ok);

        bvector.push_back(false), bvector.push_back(false);
        save_len = binary_save(bvector, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        load_len = binary_load(check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        ok = (bvector.size() == check.size());
        for (std::size_t i = 0; i < bvector.size() && ok; ++i)
            if (bvector[i] != check[i])
                ok = false;
        
        BOOST_CHECK(ok);

        const int tc = 1;

        // test property
        {
            boost::progress_timer bpt;
            for (int i = 0; i < tc; ++i)
            {
                binary_save(bvector, buf, sizeof(buf));
            }
        }
        {
            boost::progress_timer bpt;
            for (int i = 0; i < tc; ++i)
            {
                binary_load(bvector, buf, sizeof(buf));
            }
        }
    }

    {
        fake_pod_struct obj[10], check[10];
        BOOST_CHECK( Bex::serialization::is_binary_trivial<fake_pod_struct>::value );

        std::size_t save_len = binary_save(obj, buf, sizeof(buf));
        BOOST_CHECK( save_len > 0 );

        std::size_t load_len = binary_load(check, buf, sizeof(buf));
        BOOST_CHECK( load_len > 0 );

        BOOST_CHECK_EQUAL(save_len, load_len);

        BOOST_CHECK(memcmp(obj, check, sizeof(obj)) == 0);
    }

#if defined(BEX_SERIALIZATION_POD_EXTEND)
    BOOST_STATIC_ASSERT(Bex::serialization::is_binary_trivial<not_pod>::value);
#else
    BOOST_STATIC_ASSERT(!Bex::serialization::is_binary_trivial<not_pod>::value);
#endif

    BOOST_STATIC_ASSERT(Bex::serialization::has_version<static_version>::value);
    BOOST_STATIC_ASSERT(Bex::serialization::has_version<friend_version>::value);
    BOOST_STATIC_ASSERT(Bex::serialization::has_version<external_version>::value);

    BOOST_CHECK(Bex::serialization::get_version<static_version>() == 2);
    BOOST_CHECK(Bex::serialization::get_version<friend_version>() == 3);
    BOOST_CHECK(Bex::serialization::get_version<external_version>() == 4);
    
    BOOST_CHECK(Bex::serialization::get_version<not_pod>() == 0);

    XDump("结束测试 stream");
}

/// 性能测试
BOOST_AUTO_TEST_CASE(t_stream_property_case)
{
    XDump("开始测试 stream.serialization性能");

#ifdef _DEBUG
    const int tc = 10000;
#else  //_DEBUG
    const int tc = 10000 * 10;
#endif //_DEBUG

    {
        const int len = 160 * tc;
        char *buf = new char[len];
        static_streambuf ssb(buf, len);
        binary_archive bio(ssb);
        TestStreamStruct obj;

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
        pod_t pod_obj;
        static_assert(std::is_pod<pod_t>::value, "pod_t must be a 'pod' type");

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

    XDump("结束测试 stream.serialization性能");
}

BOOST_AUTO_TEST_SUITE_END()
