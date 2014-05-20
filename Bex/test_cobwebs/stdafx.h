#pragma once
#pragma warning(disable:4996)

#include <iostream>
#include <Bex/network/cobwebs.h>
#include <Bex/signals/signal.hpp>
#include <boost/typeof/typeof.hpp>
#define Dump(x) std::cout << x << std::endl;

BEX_DEFINE_SIGNAL(0, 0, void(char const*, size_t));

#include "options.h"
#include "packet_head.h"

#define TEST_MULTI_SESSION