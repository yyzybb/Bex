#ifndef __BEX_UINT3__
#define __BEX_UINT3__

#include <Bex/base.hpp>

//////////////////////////////////////////////////////////////////////////
/// 24位无符号整数. 
/// * 一般与一个8bit的内存组合, 用于32bit程序的多线程无锁优化

namespace Bex
{

#pragma pack(push, 1)
    class UInt3
    {
    private:
        volatile uint8_t _Bytes[3];

    public:
        UInt3()
        {
            _Bytes[0] = 0;
            _Bytes[1] = 0;
            _Bytes[2] = 0;
        }

        explicit UInt3(uint32_t _Val)
        {
            _Bytes[0] = (_Val >> 16) & 0xff;
            _Bytes[1] = (_Val >> 8) & 0xff;
            _Bytes[2] = _Val & 0xff;
        }

        UInt3 & operator=(UInt3 const& _Other)
        {
            _Bytes[0] = _Other._Bytes[0];
            _Bytes[1] = _Other._Bytes[1];
            _Bytes[2] = _Other._Bytes[2];
            return *this;
        }

        UInt3 & operator=(uint32_t _Val)
        {
            _Bytes[0] = (_Val >> 16) & 0xff;
            _Bytes[1] = (_Val >> 8) & 0xff;
            _Bytes[2] = _Val & 0xff;
            return *this;
        }

        operator uint32_t() const
        {
            return (((uint32_t)(_Bytes[0]) << 16) + ((uint32_t)(_Bytes[1]) << 8) + _Bytes[2]);
        }
    };

    struct UInt3_EndFill
    {
        UInt3		  u_int3;
        unsigned char u_fill;
    };

    struct UInt3_HeadFill
    {
        unsigned char u_fill;
        UInt3		  u_int3;
    };

#pragma pack(pop)

    static const UInt3 U3_MAX = UInt3(0x00ffffff);

}

#endif //__BEX_UINT3__