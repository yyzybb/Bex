#ifndef __BEX_IO_OPTIONS_HPP__
#define __BEX_IO_OPTIONS_HPP__

//////////////////////////////////////////////////////////////////////////
/// 可选配置项 options
#include <Bex/config.hpp>
#include <boost/shared_ptr.hpp>

namespace Bex { namespace bexio
{
    using namespace boost::asio;

    //////////////////////////////////////////////////////////////////////////
    /// @{ 选项
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
        //sbo_extend,         ///< 扩展发送缓冲区(暂不支持)
    };
    typedef send_buffer_overflow_em sboe;

    // 接收缓冲区溢出处理方法
    enum BEX_ENUM_CLASS receive_buffer_overflow_em
    {
        rbo_interrupt,      ///< 断开连接
        rbo_wait,           ///< 逻辑层等待
        //rbo_extend,         ///< 扩展接收缓冲区(暂不支持)
    };
    typedef receive_buffer_overflow_em rboe;
    /// @}
    //////////////////////////////////////////////////////////////////////////

    struct ssl_options;

    struct options
    {
        // 发送缓冲区溢出处理方法
        send_buffer_overflow_em sboe_;

        // 接收缓冲区溢出处理方法
        receive_buffer_overflow_em rboe_;

        // 连接成功\断开连接\收到数据 三种消息逻辑线程的响应方式
        message_logic_process_em mlpe_;

        // 工作线程数量(0表示处理器核心线程数)
        std::size_t workthread_count;

        // 发送缓冲区
        std::size_t send_buffer_size;
        static const std::size_t default_sbsize = 1024 * 8;
        static const std::size_t large_sbsize = 1024 * 1024 * 8;

        // 发送缓冲区
        std::size_t receive_buffer_size;
        static const std::size_t default_rbsize = 1024 * 8;
        static const std::size_t large_rbsize = 1024 * 8;

        // 每个封包最大长度(目前仅tcp packet/ssl packet协议有效)
        std::size_t max_packet_size;
        static const std::size_t default_max_packet_size = 1024 * 8;

        // 优雅地关闭连接可等待最大时长(毫秒 ms)
        unsigned int shutdown_timeout;

        /// ssl配置
        boost::shared_ptr<ssl_options> ssl_opts;

        /// 测试配置方案(Test)
        static options test()
        {
            static options const opts = {
                sboe::sbo_wait,
                rboe::rbo_wait,
                mlpe::mlp_derived,
                0,
                default_sbsize,
                default_rbsize,
                default_max_packet_size,
                30 * 1000
                };
            return opts;
        }

        /// 高并发、高连接数服务端推荐配置方案
        static options multi_session_server()
        {
            static options const opts = {
                sboe::sbo_interrupt,
                rboe::rbo_interrupt,
                mlpe::mlp_derived,
                0,
                default_sbsize,
                default_rbsize,
                default_max_packet_size,
                30 * 1000
                };
            return opts;
        }

        ///// 稳定性优先的推荐配置方案(暂不支持)
        //static options stability()
        //{
        //    static options const opts = {
        //        sboe::sbo_extend,
        //        rboe::rbo_extend,
        //        mlpe::mlp_derived,
        //        0,
        //        default_sbsize,
        //        default_rbsize,
        //        default_max_packet_size,
        //        30 * 1000
        //        };
        //    return opts;
        //}
    }; //struct options

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_OPTIONS_HPP__
