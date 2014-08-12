#ifndef __BEX_IO_PACKET_PARSER_HPP__
#define __BEX_IO_PACKET_PARSER_HPP__

//////////////////////////////////////////////////////////////////////////
/// pod封包头式的数据解析器

/*
* @Concept:
*   Parser
*/

#include "bexio_fwd.hpp"

namespace Bex { namespace bexio 
{
    // 包含std::size_t size(); 接口的封包头所用的Sizer
    struct sizer_packet_head
    {
        template <typename PacketHead>
        inline std::size_t operator()(PacketHead & ph) const
        {
            return ph.size();
        }
    };

    // 长度在前4个字节的封包头所用的Sizer
    struct first4_packet_head
    {
        template <typename PacketHead>
        inline std::size_t operator()(PacketHead & ph) const
        {
            return *(boost::uint32_t*)boost::addressof(ph);
        }
    };

    /// 封包解析器
    template <typename PacketHead = boost::uint32_t,    ///< 封包头类型定义
        typename Sizer = first4_packet_head,            ///< 根据封包头计算包长的接口(结果不包含包头的长度)
        typename Allocator = ::Bex::bexio::allocator<int> >
    class packet_parser
    {
    public:
        typedef PacketHead packet_head_type;
        typedef typename Allocator::template rebind<char>::other allocator;
        typedef boost::function<void(error_code const&, PacketHead*, char const*, std::size_t)> Callback;

        // 包头长度
        const std::size_t head_size { sizeof(PacketHead) };

        ~packet_parser()
        {
            if (init_.reset())
                allocator().deallocate(buf_);
        }

        // 初始化
        void initialize(std::size_t max_packet_size, Callback callback)
        {
            if (!init_.set())
                return ;

            size_ = (std::max)(max_packet_size, head_size);
            buf_ = allocator().allocate(size_);
            pos_ = 0;
            callback_ = callback;
        }

        // 压入待解析数据
        void parse(char const* data, std::size_t size)
        {
            char const* buf = data;
            std::size_t len = size;

            while (len)
            {
                if (0 == pos_)
                {
                    // 缓冲区中无数据, 直接在上层缓冲区中尝试组包.
                    if (len < head_size)
                    {
                        // 不足一个封包头, 写入缓冲区即可
                        write_buffer(buf, len);
                        return ;
                    }
                    else
                    {
                        // 有完整的封包头, 进一步检测是否有完整的封包.
                        std::size_t packet_len = Sizer()(packethead(buf)) + head_size;
                        if (packet_len > size_)
                        {
                            // 封包长错误
                            invoke_callback(make_error_code(bee::parse_error), 0, 0, 0);
                            return ;
                        }

                        if (len < packet_len)
                        {
                            // 不足一个封包, 放入缓冲区中, 等待后续数据.
                            write_buffer(buf, len);
                            return ;
                        }
                        else
                        {
                            // 有完整的封包, 直接处理.
                            invoke_callback(error_code(), &packethead(buf), buf + head_size, packet_len - head_size);
                            buf += packet_len;
                            len -= packet_len;
                            continue;
                        }
                    }
                }
                else if (pos_ < head_size)
                {
                    // 缓冲区中有数据, 但不足一个封包头, 尝试拼足一个封包头
                    std::size_t delta = head_size - pos_;
                    std::size_t cpy_size = (std::min)(delta, len);
                    write_buffer(buf, cpy_size);
                    buf += cpy_size;
                    len -= cpy_size;
                    continue;
                }
                else
                {
                    // 缓冲区中有完整的封包头
                    std::size_t packet_len = Sizer()(packethead(buf_)) + head_size;
                    if (packet_len > size_)
                    {
                        // 封包长错误
                        invoke_callback(make_error_code(bee::parse_error), 0, 0, 0);
                        return ;
                    }

                    std::size_t delta = packet_len - pos_;
                    if (delta > len)
                    {
                        // 无法拼出一个完整的封包
                        write_buffer(buf, len);
                        return ;
                    }
                    else
                    {
                        // 可以拼出一个完整的封包
                        write_buffer(buf, delta);
                        invoke_callback(error_code(), &packethead(buf_), buf_ + head_size, pos_ - head_size);
                        pos_ = 0;
                        buf += delta;
                        len -= delta;
                        continue;
                    }
                }
            }
        }

    private:
        // 调用回调
        inline void invoke_callback(error_code const& ec, PacketHead * ph, char const* data, std::size_t size)
        {
            if (callback_)
                callback_(ec, ph, data, size);
        }

        // 写入数据至缓冲区
        bool write_buffer(char const* data, std::size_t size)
        {
            if (size + pos_ > size_) 
                return false;

            memcpy(buf_ + pos_, data, size);
            pos_ += size;
            return true;
        }

        // 从数据转换为包头
        inline PacketHead & packethead(char const* data)
        {
            return (*(PacketHead*)data);
        }

    private:
        // 是否初始化
        sentry<inter_lock> init_;

        // 缓冲区
        char * buf_;
        std::size_t size_;
        std::size_t pos_;

        // 解析结果回调
        Callback callback_;
    };


} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_PACKET_PARSER_HPP__