#ifndef __BEX_IO_HANDLERS_HPP__
#define __BEX_IO_HANDLERS_HPP__

///////////////////////////////////////////////////////////////////////////
// 各种handler包装器
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    typedef shared_ptr<sentry<inter_lock> > handler_token_type;
    typedef sentry<inter_lock> origin_condition_type;
    typedef shared_ptr<origin_condition_type > handler_condition_type;

    struct EmptyHandler
    {
        template <typename ... Args>
        inline void operator()(Args && ... args) const
        {}
    };

    // 互斥式handler包装器
    template <typename Handler>
    class mutex_handler_t
    {
    public:
        mutex_handler_t(Handler const& handler, handler_token_type const& token)
            : handler_(handler), token_(token)
        {}

        handler_token_type const& get_token() const
        {
            return token_;
        }

        template <typename ... Args>
        inline void operator()(Args && ... args) const
        {
            if (token_->set())
                handler_(std::forward<Args>(args)...);
        }

    private:
        Handler handler_;
        handler_token_type token_;
    };

    template <typename Handler>
    mutex_handler_t<Handler> mutex_handler(Handler const& handler
        , handler_token_type const& token)
    {
        return mutex_handler_t<Handler>(handler, token);
    }

    // 条件式handler包装器
    template <typename IfHandler, typename ElseHandler>
    class condition_handler_t
    {
    public:
        condition_handler_t(handler_condition_type const& cond
            , IfHandler const& ifhandler
            , ElseHandler const& elsehandler)
            : ifhandler_(ifhandler), elsehandler_(elsehandler), cond_(cond)
        {}

        template <typename ... Args>
        inline void operator()(Args && ... args) const
        {
            if (cond_->is_set())
                ifhandler_(std::forward<Args>(args)...);
            else
                elsehandler_(std::forward<Args>(args)...);
        }

    private:
        IfHandler ifhandler_;
        ElseHandler elsehandler_;
        handler_condition_type cond_;
    };

    template <typename IfHandler, typename ElseHandler>
    condition_handler_t<IfHandler, ElseHandler> if_else_handler(handler_condition_type const& cond
            , IfHandler const& ifhandler
            , ElseHandler const& elsehandler)
    {
        return condition_handler_t<IfHandler, ElseHandler>(cond, ifhandler, elsehandler);
    }

    template <typename IfHandler>
    condition_handler_t<IfHandler, EmptyHandler> condition_handler(handler_condition_type const& cond
            , IfHandler const& ifhandler)
    {
        return condition_handler_t<IfHandler, EmptyHandler>(cond, ifhandler, EmptyHandler());
    }

    // 带超时的handler
    template <typename Handler, typename TimeoutHandler, typename Allocator = allocator<int> >
    class timer_handler_t
    {
    public:
        typedef Allocator allocator;
        typedef deadline_timer timer_type;
        timer_handler_t(Handler const& handler, TimeoutHandler const& timeout_handler, io_service & ios)
            : token_(make_shared_ptr<token_type, allocator>()), timer_(ios)
            , handler_(handler, token_), timeout_handler_(timeout_handler, token_)
        {}

        void async_wait(boost::posix_time::time_duration time)
        {
            timer_.expires_from_now(time);
            timer_.async_wait(timeout_handler_);
        }

        template <typename ... Args>
        inline void operator()(Args && ... args) const
        {
            handler_(std::forward<Args>(args)...);
        }

    private:
        handler_token_type token_;
        timer_type timer_;
        mutex_handler_t<Handler> handler_;
        mutex_handler_t<TimeoutHandler> timeout_handler_;
    };


} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_HANDLERS_HPP__