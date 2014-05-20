#pragma once

#include <Bex/utility/singleton.hpp>

enum test_type {
    t_do_nothing = 0,
    t_send = 1,
    t_send_and_shutdown = 2,
    t_echo = 4,
    t_packet = 8,
};

struct test_option
    : Bex::singleton<test_option>
{
    int test_mark;

    test_option()
    {
        test_mark = t_do_nothing;
    }

    inline bool test(test_type t)
    {
        return (test_mark & t) ? true : false;
    }
};

inline test_option& get_option()
{
    return test_option::getInstance();
}