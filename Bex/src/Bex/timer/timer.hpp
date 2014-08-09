#ifndef __BEX_TIMER_TIMER_HPP__
#define __BEX_TIMER_TIMER_HPP__

#include <chrono>

//////////////////////////////////////////////////////////////////////////
/// 高精度计时工具 @by C++11

namespace Bex
{
    class high_resolution_timer
    {
        typedef std::chrono::high_resolution_clock clock_type;
        typedef clock_type::time_point time_point;
        const time_point start_;

    public:
        high_resolution_timer()
            : start_(clock_type::now())
        {}

        unsigned long long count_elapsed() const
        {
            time_point current(clock_type::now());
            return (current.time_since_epoch() - start_.time_since_epoch()).count();
        }

        unsigned long long micro_elapsed() const
        {
            return count_elapsed() / 10;
        }

        unsigned long long milli_elapsed() const
        {
            return micro_elapsed() / 1000;
        }

        double elapsed() const
        {
            return (double)milli_elapsed() / 1000.0;
        }
    };

    typedef high_resolution_timer timer;

} // namespace Bex

#endif //__BEX_TIMER_TIMER_HPP__