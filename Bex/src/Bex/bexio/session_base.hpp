#ifndef __BEX_IO_SESSION_BASE_HPP__
#define __BEX_IO_SESSION_BASE_HPP__

//////////////////////////////////////////////////////////////////////////
/// 连接类基类
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    template <class SessionMgr>
    class session_base
        : public SessionMgr::hook
        , public boost::enable_shared_from_this<session_base<SessionMgr> >
        , boost::noncopyable
    {
        friend SessionMgr;

    public:
        typedef SessionMgr session_mgr_type;
        typedef typename SessionMgr::session_id id;
        
        virtual ~session_base() {}

        /// 初始化(投递接收请求)
        virtual void initialize() = 0;

        /// 优雅地关闭连接
        virtual void shutdown() = 0;

        /// 强制关闭连接
        virtual void terminate() = 0;

        // 获取id
        id get_id() const
        {
            return session_mgr_type::create_id(shared_from_this());
        }

    private:
        /// session id
        long id_;
        static volatile long svlid;
    };

    template <class SessionMgr>
    volatile long session_base<SessionMgr>::svlid = 1;

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SESSION_BASE_HPP__