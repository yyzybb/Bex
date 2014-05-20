#ifndef __BEX_NETWORK_COBWEBS_PARSER_TCP_PACKET_PARSER_H__
#define __BEX_NETWORK_COBWEBS_PARSER_TCP_PACKET_PARSER_H__

//////////////////////////////////////////////////////////////////////////
/// Tcp封包解析器

/* PacketHead有两项约束:
*    1.必须是pod类型.
*       (如果PacketHead不需要构造函数, 可以定义宏BEX_COBWEBS_CHECK_PACKET_HEAD_POD来开启编译期自动检测.)
*    2.有成员函数std::size_t size() const;, 返回封包中除包头外的数据长度.
*/

#include "packet_head_wrapper.hpp"

namespace Bex { namespace cobwebs
{
    class tcp_packet_parser
    {
        class Impl;
        Impl * m_pImpl;

    public:
        typedef boost::function<void()> OnErrorFunc;

    private:
        void initialize(std::size_t max_packet_size
            , ph_wrap_ptr pWrapper, OnErrorFunc const& onError);

    public:
        tcp_packet_parser(std::size_t max_packet_size
            , ph_wrap_ptr pWrapper, OnErrorFunc const& onError);

        ~tcp_packet_parser();

        /// 压入数据
        void sputn(char * buf, std::size_t len);

        /// 剩余的数据长度
        std::size_t overage();

        /// 提取剩余数据
        std::size_t extract_overage(char * buf, std::size_t len);
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_PARSER_TCP_PACKET_PARSER_H__