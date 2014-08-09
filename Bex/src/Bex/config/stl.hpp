#ifndef __BEX_CONFIG_STL_HPP__
#define __BEX_CONFIG_STL_HPP__

//////////////////////////////////////////////////////////////////////////
/// 整合库中所需的各版本STL的差异
#include <iosfwd>

namespace std
{

#if !defined(_MSC_VER)
    const streamoff _BADOFF = -1;
#endif

}

#endif //__BEX_CONFIG_STL_HPP__