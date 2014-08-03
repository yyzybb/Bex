#ifndef __BEX_CONFIG_WINDOWS_H__
#define __BEX_CONFIG_WINDOWS_H__

//////////////////////////////////////////////////////////////////////////
// include windows.h
#include <Bex/config/config.hpp>

#if defined(BEX_WINDOWS_API)
# include <WinSock2.h>
# include <Windows.h>
#endif //defined(BEX_WINDOWS_API)

#endif //__BEX_CONFIG_WINDOWS_H__