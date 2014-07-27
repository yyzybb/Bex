#ifndef __BEX_IO_HANDLERS_HPP__
#define __BEX_IO_HANDLERS_HPP__

///////////////////////////////////////////////////////////////////////////
// 各种handler包装器
#include "bexio_fwd.hpp"

namespace Bex { namespace bexio
{
    typedef sentry<inter_lock> origin_token_type;
    typedef shared_ptr<sentry<inter_lock> > handler_token_type;
    typedef sentry<inter_lock> origin_condition_type;
    typedef shared_ptr<origin_condition_type > handler_condition_type;

    template <typename Arg>
    void empty_function(Arg) {}

    struct EmptyHandler
    {
        typedef void result_type;

        template <typename ... Args>
        inline void operator()(Args && ... args) const {}
    };

    // 互斥式handler包装器
    template <typename Handler>
    class mutex_handler_t
    {
    public:
        typedef void result_type;

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
        typedef void result_type;

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
    template <typename Handler, typename Allocator = allocator<int> >
    class timer_handler_t
    {
    public:
        typedef Allocator allocator;
        typedef deadline_timer timer_type;
        typedef void result_type;

        timer_handler_t(Handler const& handler, io_service & ios)
            : token_(make_shared_ptr<origin_token_type, allocator>())
            , timer_(make_shared_ptr<timer_type, allocator>(ios))
            , handler_(handler, token_)
        {}

        void expires_at(boost::posix_time::ptime time)
        {
            timer_->expires_at(time);
        }

        void expires_from_now(boost::posix_time::time_duration time)
        {
            timer_->expires_from_now(time);
        }

        template <typename TimeoutHandler>
        void async_wait(BEX_MOVE_ARG(TimeoutHandler) timeout_handler)
        {
            timer_->async_wait(mutex_handler_t<TimeoutHandler>(
                BEX_MOVE_CAST(TimeoutHandler)(timeout_handler), token_));
        }

        template <typename ... Args>
        inline void operator()(Args && ... args) const
        {
            handler_(std::forward<Args>(args)...);
        }

    private:
        handler_token_type token_;
        shared_ptr<timer_type> timer_;
        mutex_handler_t<Handler> handler_;
    };

    template <typename Allocator, typename Handler>
    timer_handler_t<typename boost::remove_reference<typename boost::remove_cv<Handler>::type>::type, Allocator>
        timer_handler(BEX_MOVE_ARG(Handler) handler, io_service & ios)
    {
        return timer_handler_t<typename boost::remove_reference<typename boost::remove_cv<Handler>::type>::type, Allocator>
            (BEX_MOVE_CAST(Handler)(handler), ios);
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_HANDLERS_HPP__