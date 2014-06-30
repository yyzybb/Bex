#ifndef __BEX_IO_OPTIONS_HPP__
#define __BEX_IO_OPTIONS_HPP__

//////////////////////////////////////////////////////////////////////////
/// 可选配置项
#include <Bex/config.hpp>

namespace Bex { namespace bexio
{
    //////////////////////////////////////////////////////////////////////////
    /// @{ 选项
    // 连接成功\断开连接\收到数据 三种消息通知逻辑线程的方式
    enum BEX_ENUM_CLASS notify_logic_thread_em
    {
        nlt_reactor,        ///< reactor模式, 将消息post到指定完成队列中.
        nlt_loop,           ///< 逻辑线程轮询session
    };
    typedef notify_logic_thread_em nlte;

    // 连接成功\断开连接\收到数据 三种消息逻辑线程的响应方式
    enum BEX_ENUM_CLASS message_logic_process_em
    {
        mlp_callback,       ///< 注册回调函数
        mlp_derived,        ///< 继承sessioin并重写消息响应函数
        mlp_both,           ///< 所有方式一起生效
    };
    typedef message_logic_process_em mlpe;

    // 发送缓冲区溢出处理方法
    enum BEX_ENUM_CLASS send_buffer_overflow_em
    {
        sbo_interrupt,      ///< 断开连接
        sbo_wait,           ///< 逻辑层等待
        sbo_extend,         ///< 扩展发送缓冲区(暂不支持)
    };
    typedef send_buffer_overflow_em sboe;

    // 接收缓冲区溢出处理方法
    enum BEX_ENUM_CLASS receive_buffer_overflow_em
    {
        rbo_interrupt,      ///< 断开连接
        rbo_wait,           ///< 接收层等待
        rbo_extend,         ///< 扩展接收缓冲区(暂不支持)
    };
    typedef receive_buffer_overflow_em rboe;
    /// @}
    //////////////////////////////////////////////////////////////////////////

    struct options
    {
        // 连接成功\断开连接\收到数据 三种消息通知逻辑线程的方式
        notify_logic_thread_em nlte_;

        // 发送缓冲区溢出处理方法
        send_buffer_overflow_em sboe_;

        // 接收缓冲区溢出处理方法
        receive_buffer_overflow_em rboe_;

        // 连接成功\断开连接\收到数据 三种消息逻辑线程的响应方式
        message_logic_process_em mlpe_;

        /// 测试配置方案(Test)
        static options test()
        {
            static options const opts = {
                nlte::nlt_reactor,
                sboe::sbo_wait,
                rboe::rbo_wait,
                mlpe::mlp_derived
                };
            return opts;
        }

        /// 高并发、高连接数服务端推荐配置方案
        static options multi_session_server()
        {
            static options const opts = {
                nlte::nlt_reactor,
                sboe::sbo_interrupt,
                rboe::rbo_interrupt,
                mlpe::mlp_derived
                };
            return opts;
        }

        /// 稳定性优先的推荐配置方案
        static options stability()
        {
            static options const opts = {
                nlte::nlt_reactor,
                sboe::sbo_extend,
                rboe::rbo_extend,
                mlpe::mlp_derived
                };
            return opts;
        }

        /// 低连接数、高吞吐量的推荐配置方案(客户端很适合使用这个)
        static options throughput()
        {
            static options const opts = {
                nlte::nlt_loop,
                sboe::sbo_extend,
                rboe::rbo_extend,
                mlpe::mlp_derived
                };
            return opts;
        }
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_OPTIONS_HPP__
