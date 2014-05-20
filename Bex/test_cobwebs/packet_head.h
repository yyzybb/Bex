#pragma once

#pragma pack(push, 1)
struct _stPacketHead
{
    char _1[5];
    unsigned int buf_size;
    char _2[7];

    _stPacketHead() : buf_size(0)
    {
    }

    unsigned int size() const
    {
        return buf_size;
    }
};
#pragma pack(pop)