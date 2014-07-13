#ifndef __BEX_IO_BUFFERED_SOCKET_HPP__
#define __BEX_IO_BUFFERED_SOCKET_HPP__

//////////////////////////////////////////////////////////////////////////
/// socket之上封装一层收发缓冲区

#include "bexio_fwd.hpp"
#include "allocator.hpp"

namespace Bex { namespace bexio
{
    template <typename Socket, typename Buffer, typename Allocator>
    class buffered_socket
        : boost::noncopyable
    {
        typedef typename Allocator::template rebind<char>::other alloc_t;
        typedef buffered_socket<Socket, Buffer, Allocator> this_type;

    public:
        typedef typename boost::remove_reference<Socket>::type next_layer_type;
        typedef typename lowest_layer_t<Socket>::type lowest_layer_type;
        typedef boost::function<void(error_code, std::size_t)> callback_t;

    public:
        // @rbsize : 接收缓冲区大小
        // @wbsize : 发送缓冲区大小
        template <typename Arg>
        buffered_socket(Arg & arg, std::size_t rbsize, std::size_t wbsize)
            : socket_(arg)
            , read_buffer_(alloc_t().allocate(rbsize), rbsize)
            , write_buffer_(alloc_t().allocate(wbsize), wbsize)
        {
            read_storage_ = read_buffer_.address();
            write_storage_ = write_buffer_.address();
        }

        template <typename Arg1, typename Arg2>
        buffered_socket(Arg1 & arg1, Arg2 & arg2, std::size_t rbsize, std::size_t wbsize)
            : socket_(arg1, arg2)
            , read_buffer_(alloc_t().allocate(rbsize), rbsize)
            , write_buffer_(alloc_t().allocate(wbsize), wbsize)
        {
            read_storage_ = read_buffer_.address();
            write_storage_ = write_buffer_.address();
        }

        ~buffered_socket()
        {
            alloc_t().deallocate((void*)read_storage_);
            alloc_t().deallocate((void*)write_storage_);
        } 
        
        // 获取下一层
        next_layer_type& next_layer()
        {
            return socket_;
        }

        // 获取最底层
        lowest_layer_type& lowest_layer()
        {
            return ::Bex::bexio::lowest_layer(socket_);
        }

        // nothing to speak...
        io_service& get_io_service()
        {
            return socket_.get_io_service();
        }

        // 关闭
        void close()
        {
            socket_.close();
        }

        // 关闭
        error_code close(error_code & ec)
        {
            return socket_.close(ec);
        }

        // 同步写入(发送)数据
        template <typename ConstBufferSequence>
        std::size_t write_some(const ConstBufferSequence& buffers)
        {
            return socket_.write_some(buffers);
        }

        // 同步写入(发送)数据
        template <typename ConstBufferSequence>
        std::size_t write_some(const ConstBufferSequence& buffers, error_code& ec)
        {
            return socket_.write_some(buffers, ec);
        }

        // 异步写入(发送)数据
        // @WriteHandler : void(boost::system::error_code, std::size_t))
        template <typename WriteHandler>
        bool async_write_some(WriteHandler const& handler)
        {
            boost::array<const_buffer, 2> buffers;
            if (!write_buffer_.get_buffers(buffers))
                return false;

            socket_.async_write_some(buffers,
                BEX_IO_BIND(&this_type::on_async_write, this
                    , BEX_IO_PH_ERROR, BEX_IO_PH_BYTES_TRANSFERRED, callback_t(handler)));
            return true;
        }

        // 同步读取(接收)数据
        template <typename MutableBufferSequence>
        std::size_t read_some(const MutableBufferSequence& buffers)
        {
            return socket_.read_some(buffers);
        }

        // 同步读取(接收)数据
        template <typename MutableBufferSequence>
        std::size_t read_some(const MutableBufferSequence& buffers,
            boost::system::error_code& ec)
        {
            return socket_.read_some(buffers, ec);
        }

        // 异步读取(接收)数据
        // @ReadHandler : void(boost::system::error_code, std::size_t)
        template <typename ReadHandler>
        bool async_read_some(ReadHandler const& handler)
        {
            boost::array<mutable_buffer, 2> buffers;
            if (!read_buffer_.put_buffers(buffers))
                return false;

            socket_.async_read_some(buffers,
                BEX_IO_BIND(&this_type::on_async_read, this
                        , BEX_IO_PH_ERROR, BEX_IO_PH_BYTES_TRANSFERRED, callback_t(handler)));
            return true;
        }

        // 提取接收缓冲区中的已收到的数据
        template <typename ConstBufferSequence>
        std::size_t get_buffers(ConstBufferSequence & buffers) const
        {
            return read_buffer_.get_buffers(buffers);
        }

        // 读取数据完成
        void read_done(std::size_t size)
        {
            read_buffer_.gbump(size);
        }

        // 接收缓冲区可读数据长度
        std::size_t getable_read() const
        {
            return read_buffer_.gsize();
        }

        // 接收缓冲区可写数据长度
        std::size_t putable_read() const
        {
            return read_buffer_.psize();
        }

        // 提取发送缓冲区中的可写数据段
        template <typename MutableBufferSequence>
        std::size_t put_buffers(MutableBufferSequence & buffers) const
        {
            return write_buffer_.put_buffers(buffers);
        }

        // 写入数据完成
        void write_done(std::size_t size)
        {
            write_buffer_.pbump(size);
        }

        // 发送缓冲区可读数据长度
        std::size_t getable_write() const
        {
            return write_buffer_.gsize();
        }

        // 发送缓冲区可写数据长度
        std::size_t putable_write() const
        {
            return write_buffer_.psize();
        }

        // 将要发送的数据写入发送缓冲区中
        std::size_t sputn(char const* data, std::size_t size)
        {
            return write_buffer_.sputn(data, size);
        }

        template <typename ConstBufferSequence>
        std::size_t sputn(ConstBufferSequence const& buffers)
        {
            return write_buffer_.sputn_to_buffers(buffers);
        }

        // 将接收缓冲区中已接收到数据copy到MutableBufferSequence中
        std::size_t sgetn(char const* data, std::size_t size)
        {
            return read_buffer_.sgetn(data, size);
        }

        template <typename MutableBufferSequence>
        std::size_t sgetn(MutableBufferSequence & buffers)
        {
            return read_buffer_.sgetn_to_buffers(buffers);
        }

    private:
        void on_async_write(error_code ec, std::size_t bytes_transferred
            , callback_t const& handler)
        {
            if (!ec)
                write_buffer_.gbump(bytes_transferred);
            handler(ec, bytes_transferred);
        }

        void on_async_read(error_code ec, std::size_t bytes_transferred
            , callback_t const& handler)
        {
            if (!ec)
                read_buffer_.pbump(bytes_transferred);
            handler(ec, bytes_transferred);
        }

    private:
        Socket socket_;
        const char * read_storage_;
        const char * write_storage_;
        Buffer read_buffer_;
        Buffer write_buffer_;
    };
    
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BUFFERED_SOCKET_HPP__