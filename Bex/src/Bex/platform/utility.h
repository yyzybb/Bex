#ifndef __BEX_PLATFORM_UTILITY_H__
#define __BEX_PLATFORM_UTILITY_H__

#include <boost/logic/tribool.hpp>
#include <Bex/config/windows.h>

namespace Bex
{
    // 当前执行的代码是否在Dll中.
    inline boost::tribool is_in_dll()
#if defined(BEX_WINDOWS_API) && defined(_APISETLIBLOADER_)
    {
        HMODULE hBase = ::GetModuleHandleW(NULL), hThis = 0;
        BOOL bOk = ::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
            , (LPCWSTR)&::Bex::is_in_dll, &hThis);
        if (!hThis || !bOk)
            return boost::tribool(boost::indeterminate);
        else
            return boost::tribool(hBase != hThis ? true : false);
    }
#else //defined(BEX_WINDOWS_API)
    {
        return boost::tribool(boost::indeterminate);
    }
#endif //defined(BEX_WINDOWS_API)

} //namespace Bex

#endif //__BEX_PLATFORM_UTILITY_H__