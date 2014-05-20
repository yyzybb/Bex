#ifndef __BEX_NETWORK_COBWEBS_SESSION_BASE_HPP__
#define __BEX_NETWORK_COBWEBS_SESSION_BASE_HPP__

//////////////////////////////////////////////////////////////////////////
/// session基类

#include <Bex/network/cobwebs/core/core.h>
#include <Bex/network/cobwebs/core/id.h>
#include <Bex/network/cobwebs/core/options.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

namespace Bex { namespace cobwebs
{
    struct session_initialized
    {
        session_id sid;
        group_id gid;
        options_ptr opts;

        session_initialized(session_id _sid, group_id _gid, options_ptr _opts)
            : sid(_sid), gid(_gid)
        {
            if (_opts)
                opts = _opts;
            else
                opts.reset(new options());
        }

        session_initialized(session_id _sid, group_id _gid, options const& _opts)
            : sid(_sid), gid(_gid)
        {
            opts.reset(new options(_opts));
        }
    };

    class session_base
        : public boost::noncopyable
    {
    protected:
        group_id    m_gid;
        session_id  m_sid;
        options_ptr m_opts;
        io_service& m_ios;
        bool        m_valid;

    public:
        explicit session_base(session_initialized const& si)
            : m_ios(core::getInstance().get_io_service())
            , m_sid(si.sid), m_gid(si.gid), m_opts(si.opts)
            , m_valid(true)
        {
        }

        virtual ~session_base() 
        {
        }

        /// 初始化
        virtual void initialize() {}

        /// 接收线程推进
        virtual void run() = 0;

        /// 优雅地关闭连接
        virtual void shutdown() = 0;

        /// 强制关闭
        virtual void close() = 0;

        /// 是否失效
        inline bool is_valid()
        {
            return m_valid;
        }
    };

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_SESSION_BASE_HPP__