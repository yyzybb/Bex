#if defined(_WIN32)

#include "utility.h"
#include <Windows.h>

namespace Bex
{
    boost::tribool is_in_dll()
    {
        HMODULE hBase = ::GetModuleHandleW(NULL), hThis = 0;
        BOOL bOk = ::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
            , (LPCWSTR)&is_in_dll, &hThis);
        if (!hThis || !bOk)
            return boost::tribool(boost::indeterminate);
        else
            return boost::tribool(hBase != hThis ? true : false);
    }

} //namespace Bex

#endif //_WIN32