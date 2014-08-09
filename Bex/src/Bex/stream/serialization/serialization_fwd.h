#ifndef __BEX_STREAM_SERIALIZATION_SERIALIZATION_FWD_HPP__
#define __BEX_STREAM_SERIALIZATION_SERIALIZATION_FWD_HPP__

//////////////////////////////////////////////////////////////////////////
/// stream.serialization库前置声明文件

#ifndef BEX_STREAM_SERIALIZATION_VERSION_NAME
#define BEX_STREAM_SERIALIZATION_VERSION_NAME bex_serialization_version
#endif //BEX_STREAM_SERIALIZATION_VERSION_NAME

/// 用于定义结构体序列化版本号的类内枚举名
#define BEX_SS_VERSION BEX_STREAM_SERIALIZATION_VERSION_NAME

/// 消除从基类继承过来的版本号
#define BEX_SS_UNDEF_BASE_VERSION(Base) private: using Base::BEX_SS_VERSION;

namespace Bex { namespace serialization
{
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