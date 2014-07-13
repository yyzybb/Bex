#ifndef __BEX_IO_OPTIONS_HPP__
#define __BEX_IO_OPTIONS_HPP__

//////////////////////////////////////////////////////////////////////////
/// 可选配置项 options
#include <Bex/config.hpp>
#include <boost/asio/ssl/context_base.hpp>
#include <boost/asio/ssl/verify_mode.hpp>

namespace Bex { namespace bexio
{
    using namespace boost::asio;

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

    struct ssl_options
    {
        // 上下文选项
        // * 默认值: boost::asio::ssl::context_base::default_workarounds | boost::asio::ssl::context_base::no_sslv2
        // * 使用方法:
        //      @begin code:
        //          ctx.set_options(ctx_opt);
        //      @end code.
        long ctx_opt;

        // 认证要求
        // * 默认值: boost::asio::ssl::verify_none
        // * 使用方法:
        //      @begin code:
        //          ctx.set_verify_mode(verify_mode);
        //      @end code.
        long verify_mode;

        // 证书文件 (server端必须, client端可选)
        // * 默认值: "server.pem"
        // * 使用方法:
        //      @begin code:
        //          ctx.use_certificate_chain_file(crt_file);
        //      @end code.
        std::string crt_file;

        // 私钥文件 & 私钥文件类型
        // * 默认值: "server.pem", boost::asio::ssl::context_base::pem
        // * 使用方法:
        //      @begin code:
        //          ctx.use_private_key_file(pri_key_file.c_str(), file_fmt);
        //      @end code.
        std::string pri_key_file;
        ssl::context_base::file_format file_fmt;

        // dh算法密钥文件
        // * 默认值: "dh512.pem"
        // * 使用方法:
        //      @begin code:
        //          ctx.use_tmp_dh_file(dh_file);
        //      @end code.
        std::string dh_file;

        // 加密算法链
        // * 默认值: "RC4-MD5"
        // * 使用方法:
        //      @begin code:
        //    SSL_CTX_set_cipher_list(ctx.native_handle(), cipher_list.c_str());
        //      @end code.
        std::string cipher_list;

        static ssl_options client()
        {
            static ssl_options opts = {
                ssl::context_base::default_workarounds | ssl::context_base::no_sslv2,
                ssl::verify_none,
                "",
                "",
                ssl::context_base::pem,
                "dh512.pem",
                "RC4-MD5"
            };
            return opts;
        }

        static ssl_options server()
        {
            static ssl_options opts = {
                ssl::context_base::default_workarounds | ssl::context_base::no_sslv2,
                ssl::verify_none,
                "bexio.crt",
                "bexio.pem",
                ssl::context_base::pem,
                "dh512.pem",
                "RC4-MD5"
            };
            return opts;
        }
        //////////////////////////////////////////////////////////////////////////
        /// @{ sample
        // @begin code: client peer
        //
        //    ctx.set_options(boost::asio::ssl::context::default_workarounds
        //        | boost::asio::ssl::context::no_sslv2);
        //    ctx.set_verify_mode(boost::asio::ssl::verify_none);
        //
        // @end code;
        //
        // @begin code: server peer
        //
        //    ctx.set_options(boost::asio::ssl::context::default_workarounds
        //        | boost::asio::ssl::context::no_sslv2
        //        /*| boost::asio::ssl::context::single_dh_use*/ );
        //
        //    ctx.use_certificate_chain_file("server.pem");
        //    ctx.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
        //    SSL_CTX_set_cipher_list(ctx.native_handle(), "RC4-MD5");
        //
        // @end code;
        /// @}
        //////////////////////////////////////////////////////////////////////////
    };

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

        /// ssl配置
        ssl_options ssl_opts;

        /// 测试配置方案(Test)
        static options test()
        {
            static options const opts = {
                nlte::nlt_reactor,
                sboe::sbo_wait,
                rboe::rbo_wait,
                mlpe::mlp_derived,
                0,
                default_sbsize,
                default_rbsize,
                default_max_packet_size
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
                mlpe::mlp_derived,
                0,
                default_sbsize,
                default_rbsize,
                default_max_packet_size
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
                mlpe::mlp_derived,
                0,
                default_sbsize,
                default_rbsize,
                default_max_packet_size
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
                mlpe::mlp_derived,
                0,
                large_sbsize,
                large_rbsize,
                default_max_packet_size
                };
            return opts;
        }
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_OPTIONS_HPP__
