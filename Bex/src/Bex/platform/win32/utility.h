#ifndef __BEX_PLATFORM_WIN32_UTILITY_H__
#define __BEX_PLATFORM_WIN32_UTILITY_H__

#if defined(_WIN32)

#include <boost/logic/tribool.hpp>

namespace Bex
{
    // 当前执行的代码是否在Dll中.
    boost::tribool is_in_dll();

} //namespace Bex

#endif //_WIN32

#endif //__BEX_PLATFORM_WIN32_UTILITY_H__