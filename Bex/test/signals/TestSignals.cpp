#include "TestPCH.h"
#include <Bex/signals/signal.hpp>


enum
{
    SGL_Global,
};
enum
{
    SGL_Global_GetLast,
    SGL_Global_Calc,
    SGL_Global_Call,
};
BEX_DEFINE_SIGNAL(SGL_Global, SGL_Global_GetLast, int());
BEX_DEFINE_COMBINE_SIGNAL(SGL_Global, SGL_Global_Calc, unsigned int(unsigned int), Bex::combine_accumulate_value);
BEX_DEFINE_SIGNAL(SGL_Global, SGL_Global_Call, void());

class Player
{
    std::list<signal_scoped> m_sc;
    int m_index;
    static int s_index;

public:
    Player()
        : m_index(s_index++)
    {
        m_sc.push_back(make_signal_scoped(
            BEX_GET_SIGNAL(SGL_Global, SGL_Global_GetLast).connect(
            boost::bind(&Player::ret, this)
            )));

        m_sc.push_back(make_signal_scoped(
            BEX_GET_SIGNAL(SGL_Global, SGL_Global_Calc).connect(
            boost::bind(&Player::acc, this, _1)
            )));

        m_sc.push_back(make_signal_scoped(
            BEX_GET_SIGNAL(SGL_Global, SGL_Global_Call).connect(
            boost::bind(&Player::dump, this)
            )));
    }

    int ret()
    {
        return m_index;
    }

    unsigned int acc(unsigned int value)
    {
        return value;
    }

    void dump()
    {
        std::cout << "Player " << m_index << std::endl;
    }
};

int Player::s_index = 0;

BOOST_AUTO_TEST_SUITE(s_signals)
/// 正确性测试
BOOST_AUTO_TEST_CASE(t_signals)
{
    Player p0, p1, p2;

    {
        int ret = BEX_GET_SIGNAL(SGL_Global, SGL_Global_GetLast)();
        BOOST_CHECK_EQUAL(ret, 2);
    }

    {
        unsigned int value = 5;
        unsigned int ret = BEX_GET_SIGNAL(SGL_Global, SGL_Global_Calc)(value);
        BOOST_CHECK_EQUAL(ret, 3 * value);
    }

    {
        Player p3;

        {
            int ret = BEX_GET_SIGNAL(SGL_Global, SGL_Global_GetLast)();
            BOOST_CHECK_EQUAL(ret, 3);
        }

        {
            unsigned int value = 5;
            unsigned int ret = BEX_GET_SIGNAL(SGL_Global, SGL_Global_Calc)(value);
            BOOST_CHECK_EQUAL(ret, 4 * value);
        }
    }

    BEX_GET_SIGNAL(SGL_Global, SGL_Global_Call)();
}

BOOST_AUTO_TEST_SUITE_END()