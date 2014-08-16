#ifndef __BEX_IO_HTTP_PROTOCOL_HPP__
#define __BEX_IO_HTTP_PROTOCOL_HPP__

//////////////////////////////////////////////////////////////////////////
/// http protocol
#include "bexio_fwd.hpp"
#include <Bex/bexio/http/http_options.hpp>
#include <Bex/bexio/http/url.hpp>

namespace Bex { namespace bexio
{
    template <class NextLayerProtocol>
    struct http_protocol
    {
        typedef NextLayerProtocol next_layer_type;
        typedef typename lowest_layer_t<NextLayerProtocol>::type lowest_layer_type;
        typedef typename next_layer_type::socket socket;
        typedef shared_ptr<socket> socket_ptr;

        // callback functions
        typedef boost::function<void(http::http_header &, char const*, std::size_t)> OnReceiveF;

    protected:
        virtual void on_receive(http::http_header &, char const*, std::size_t) {}

        //////////////////////////////////////////////////////////////////////////
        /// @{ 只有session可以调用这个接口
    public:
        /// 初始化
        template <typename F, typename Id>
        void initialize(shared_ptr<options> const& opts, F const& f, Id const& id)
        {
            opts_ = opts;
            if (f)
                global_receiver_ = BEX_IO_BIND(f, id, _1, _2, _3);
        }

    protected:
        /// 用于传递接收到的数据至解析器
        inline void parse(const_buffer const& buffer)
        {
        }

        inline void on_parse(http::http_header & header, char const* data, std::size_t size)
        {
            if (opts_->mlpe_ == mlpe::mlp_derived || opts_->mlpe_ == mlpe::mlp_both)
                on_receive(header, data, size);
            
            if (opts_->mlpe_ == mlpe::mlp_callback || opts_->mlpe_ == mlpe::mlp_both)
                if (global_receiver_)
                    global_receiver_(header, data, size);
        }

    protected:
        shared_ptr<options> opts_;
        OnReceiveF global_receiver_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_HTTP_PROTOCOL_HPP__