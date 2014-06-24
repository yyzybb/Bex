#ifndef __BEX_IO_MULTITHREAD_STRAND_SERVICE_HPP__
#define __BEX_IO_MULTITHREAD_STRAND_SERVICE_HPP__

//////////////////////////////////////////////////////////////////////////
// 多线程->单线程串行服务, 组合至io_service中以增强其功能。
/*
* @
*/

#include "bexio_fwd.hpp"
#include "multithread_strand.hpp"

namespace Bex { namespace bexio
{
    class multithread_strand_service
        : public detail::service_base<multithread_strand_service>
        , public multithread_strand<io_service&>
    {
        typedef detail::service_base<multithread_strand_service> base_type;

    public:
        explicit multithread_strand_service(io_service& owner)
            : base_type(owner)
        {
        }

    private:
        // 关闭服务
        virtual void shutdown_service()
        {
            shutdown();
        }
    };


} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_MULTITHREAD_STRAND_SERVICE_HPP__