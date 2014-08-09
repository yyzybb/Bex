#ifndef __TEST_PCH__
#define __TEST_PCH__

#pragma warning(disable:4996)
#pragma warning(disable:4101)
#pragma warning(disable:4819)

#include <iostream>
#include <stdio.h>
#include <stdarg.h>

#include <vector>
#include <map>
#include <list>
#include <string>
#include <queue>
#include <algorithm>

#include <Bex/base.hpp>

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>

#include <boost/mpl/list.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>

#include <Bex/platform.hpp>
#include <Bex/base.hpp>
#include <Bex/config.hpp>
#include <Bex/functor.hpp>
using namespace Bex;

#include "TestCommon.hpp"

#endif //__TEST_PCH__
