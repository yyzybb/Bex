#ifndef __BEX_NETWORK_COBWEBS_CORE_DELETER_HPP__
#define __BEX_NETWORK_COBWEBS_CORE_DELETER_HPP__

//////////////////////////////////////////////////////////////////////////
/// 引用计数归一时清理对象, 转移智能指针指向的对象的析构时机.

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <map>
#include "core.h"

namespace Bex { namespace cobwebs
{
    template <typename T>
    class core_deleter;

    /// delete操作转移到core的主循环中处理
    template <typename T>
    struct mainloop_deleter
    {
        inline void operator()(T * ptr) const
        {
            core::getInstance().post(core_deleter<T>(ptr));
        }
    };

    /// *** delete仿函数, 为追求效率, 安全性不足, 慎用!!!
    template <typename T>
    class core_deleter
    {
        T * m_pointer;

    public:
        explicit core_deleter(T * pointer)
            : m_pointer(pointer)
        {
        }

        inline void operator()() const
        {
            delete m_pointer;
        }
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_CORE_DELETER_HPP__