#ifndef __BEX_NETWORK_COBWEBS_OPTIONS_H__
#define __BEX_NETWORK_COBWEBS_OPTIONS_H__

//////////////////////////////////////////////////////////////////////////
/// 选项配置

#include <Bex/utility/singleton.hpp>

namespace Bex { namespace cobwebs
{
    struct options
    {
        int sendbufbytes;   ///< 发送缓冲区大小
        int recvbufbytes;   ///< 接收缓冲区大小
        int sendbufcount;   ///< 发送缓冲区个数
        int recvbufcount;   ///< 接收缓冲区个数
        bool sendbufoverflow_disconnect;  ///< 发送缓冲区溢出是否断开连接
        bool recvbufoverflow_disconnect;  ///< 接收缓冲区溢出是否断开连接
        int max_packet_size;    ///< 单个数据包最大长度
        int listen_count;       ///< 监听的数量

        options()
        {
            sendbufbytes = 64 * 1024;
            recvbufbytes = 64 * 1024;
            sendbufcount = 1;
            recvbufcount = 1;
            sendbufoverflow_disconnect = true;
            recvbufoverflow_disconnect = true;
            max_packet_size = 8 * 1024;
            listen_count = 1;
        }

        /// Todo: read from ini/xml/property_tree
    };

    typedef boost::shared_ptr<options> options_ptr;

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_OPTIONS_H__