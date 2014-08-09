#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "TestPCH.h"
//#include <boost/test/included/unit_test.hpp>
#include <boost/thread.hpp>
#include <stdio.h>

void my_pause()
{
#ifdef __MSVC
    system("pause");
#else //__MSVC
    Dump("Press any key to continue!");
    std::cin.get();
#endif //__MSVC
}

class TestMain
{
public:
    TestMain()
    {
        Dump("Bex Test Start...");
        atexit(&my_pause);
    }

    ~TestMain()
    {
        //my_pause();
    }
};

BOOST_GLOBAL_FIXTURE(TestMain);

BOOST_AUTO_TEST_SUITE(s_main)

BOOST_AUTO_TEST_CASE(t_main)
{

}

BOOST_AUTO_TEST_SUITE_END()
