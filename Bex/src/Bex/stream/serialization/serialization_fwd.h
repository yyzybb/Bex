#ifndef __BEX_STREAM_SERIALIZATION_SERIALIZATION_FWD_HPP__
#define __BEX_STREAM_SERIALIZATION_SERIALIZATION_FWD_HPP__

//////////////////////////////////////////////////////////////////////////
/// stream.serialization库前置声明文件
//
// * 2014-08-17 @yyz: 1.自定义版本号改为member+adl的方式, 
//                      使用在自定义类外部定义版本号或friend的方式可以防止版本号被继承.
//                      使用member函数方式定义版本号, 可以实现单根公有继承体系共享版本号.
//                    2.不再支持C++98, 以获得更精确的静态检测功能.
//                    3.增加config.h文件, 以便于用户配置序列化库的行为.
//
// * 2014-08-17 @yyz: 提供用于数据持久化的序列化接口(后缀_persisitence), 所有要使用此接口做序列化和反序列化的非内置类型都要有序列化接口和版本号定义.
//
// @Todo: 增加nvp, 支持xml json bson等格式, 无nvp的情况下, 默认使用类型名做KEY.
// @Todo: 重新规范化text_archive的格式.

#include <Bex/config.hpp>
#include <Bex/type_traits/type_traits.hpp>
#include <Bex/utility/exception.h>
#include <Bex/math/compress_numeric.hpp>
#include <iosfwd>

// stl containers adapter
#include <vector> // vector<T> and vector<bool>
#include <set>
#include <list>
#include <string>
#include <map>
#include <deque>

#if defined(_MSC_VER)
# include <hash_map>
#endif //defined(_MSC_VER)

#if defined(BOOST_HAS_TR1_UNORDERED_MAP)
# include <unordered_map>
#endif //defined(BOOST_HAS_TR1_UNORDERED_MAP)

#if defined(BOOST_HAS_TR1_UNORDERED_SET)
# include <unordered_set>
#endif //defined(BOOST_HAS_TR1_UNORDERED_SET)

#if defined(BEX_SUPPORT_CXX11)
# include <array>
#endif //defined(BEX_SUPPORT_CXX11)

// boost containers adapter
#include <boost/bimap.hpp>
#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits.hpp>
#include <boost/io/ios_state.hpp>

#include "config.h"
#include "wrappers.hpp"

namespace Bex { namespace serialization
{
    //////////////////////////////////////////////////////////////////////////
    /// archive states
    enum class archive_state 
        : unsigned char
    {
        idle,
        running,
        error,
    };
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// archive mark
    enum archive_mark_bit
    {
        amb_none = 0x0,
        amb_rollback = 0x1,        ///< 如果reback被设置, 读/写失败时, 数据流会还原至操作前的状态和位置.
    };
    typedef int archive_mark;
    static const archive_mark default_mark = amb_none;

    //////////////////////////////////////////////////////////////////////////
    /// @{ Archive类型枚举
    enum BEX_ENUM_CLASS archive_mode_enum
    {
        archive_mode_binary,
        archive_mode_text,
        archive_mode_unkown,
    };
    struct binary_mode_tag {};
    struct text_mode_tag {};
    struct unkown_mode_tag {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ Archive操作类型枚举
    enum BEX_ENUM_CLASS archive_oper_enum
    { 
        archive_oper_load, 
        archive_oper_save, 
        archive_oper_unkown,
    };
    struct load_oper_tag {};
    struct save_oper_tag {};
    struct unkown_oper_tag {};
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 数据包装类
    /// 二进制数据流包装类
    struct binary_wrapper;

    /// 文本数据流包装类
    template <typename T>
    struct text_wrapper;
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// binary archive base
    struct binary_base {};

    /// text archive base
    struct text_base {};

    /// i/o base
    struct input_archive_base {};
    struct output_archive_base {};

    /// archive traits
    template <class Archive>
    struct archive_traits;

    /// 判断类型是否已特化为配接器
    template <typename T>
    struct is_adapter_type;
    //////////////////////////////////////////////////////////////////////////

} //namespace serialization

namespace {
    using serialization::archive_mark;
    using serialization::amb_none;
    using serialization::amb_rollback;

    using serialization::load_oper_tag;
    using serialization::save_oper_tag;
    using serialization::unkown_oper_tag;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_SERIALIZATION_FWD_HPP__
