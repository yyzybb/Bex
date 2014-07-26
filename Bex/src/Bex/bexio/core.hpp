#ifndef __BEX_IO_BEXIO_CORE_HPP__
#define __BEX_IO_BEXIO_CORE_HPP__

//////////////////////////////////////////////////////////////////////////
/// 核心层(对io_service的封装)
#include "bexio_fwd.hpp"
#include "multithread_strand_service.hpp"

namespace Bex { namespace bexio
{
    struct core_default_tag {};

    template <class Allocator = ::Bex::bexio::allocator<int>
        , class Tag = ::Bex::bexio::core_default_tag
        >
    class core
        : public singleton<core<Allocator, Tag> >
    {
        friend class singleton<core<Allocator, Tag> >;

    public:
        typedef multithread_strand_service<Allocator> ServiceType;

        // 主循环推进
        std::pair<std::size_t, error_code> run()
        {
            return mts_srv_.run();
        }

        // 获得后端io_service. 
        // * 网络层使用的, 
        // * 初始化server/client时, 请务必使用这个接口返回的io_service, 且Allocator必须一致!
        io_service & backend()
        {
            return ios_;
        }

        // 获得前端io_service. 
        // * 应用层使用的
        io_service & backfront()
        {
            return mts_srv_.actor();
        }

        // 返回服务
        ServiceType & get_service()
        {
            return mts_srv_;
        }

    private:
        core()
            : mts_srv_(use_service<ServiceType>(ios_))
        {
        }

        ~core() {}

    private:
        io_service ios_;
        ServiceType & mts_srv_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_BEXIO_CORE_HPP__