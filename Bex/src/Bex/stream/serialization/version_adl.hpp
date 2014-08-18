#ifndef __BEX_STREAM_SERIALIZATION_VERSION_ADL_HPP__
#define __BEX_STREAM_SERIALIZATION_VERSION_ADL_HPP__

//////////////////////////////////////////////////////////////////////////
/// 用户自定义类型的版本号
#include "serialization_fwd.h"

namespace Bex { namespace serialization
{
    class serialize_version_helper
    {
        struct unsigned_int_t
        {
            unsigned int ui_;
            unsigned_int_t(unsigned int ui) : ui_(ui) {}
            operator unsigned int() const { return ui_; }
        };

        template <typename T>
        friend unsigned_int_t serialize_version(T*);
    };

    template <typename T>
    serialize_version_helper::unsigned_int_t serialize_version(T* pt)
    {
        return pt->serialize_version();
    }

    template <typename T>
    unsigned int serialize_version_adl(T* pt)
    {
        return serialize_version(pt);
    }

} //namespace serialization
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_VERSION_ADL_HPP__