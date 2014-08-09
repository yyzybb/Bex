#include "TestPCH.h"
#include <Bex/bexio/bexio_fwd.hpp>
using namespace Bex::bexio;

struct Empty {};
struct Full
{
    typedef int next_layer_type;
    typedef int lowest_layer_type;

    next_layer_type & next_layer()
    {
        static next_layer_type obj;
        return obj;
    }

    lowest_layer_type & lowest_layer()
    {
        static lowest_layer_type obj;
        return obj;
    }
};

BOOST_AUTO_TEST_SUITE(s_bexio)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_bexio_layer_meta)
{
    XDump("开始测试 bexio_layer");

    typedef next_layer_t<Empty>::type next_Empty;
    typedef lowest_layer_t<Empty>::type lowest_Empty;
    BOOST_STATIC_ASSERT((boost::is_same<next_Empty, Empty>::value));
    BOOST_STATIC_ASSERT((boost::is_same<lowest_Empty, Empty>::value));

    typedef next_layer_t<Full>::type next_Full;
    typedef lowest_layer_t<Full>::type lowest_Full;
    BOOST_STATIC_ASSERT((boost::is_same<next_Full, int>::value));
    BOOST_STATIC_ASSERT((boost::is_same<lowest_Full, int>::value));

    Empty e;
    Full f;
    BOOST_CHECK(sizeof(next_layer(e)) == sizeof(Empty));
    BOOST_CHECK(sizeof(lowest_layer(e)) == sizeof(Empty));
    BOOST_CHECK(sizeof(next_layer(f)) == sizeof(int));
    BOOST_CHECK(sizeof(lowest_layer(f)) == sizeof(int));
    (void)e, (void)f;

    XDump("结束测试 bexio_layer");
}

BOOST_AUTO_TEST_SUITE_END()