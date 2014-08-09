#ifndef __BEX_SIGNALS_COMBINE_LAST_VALUE__
#define __BEX_SIGNALS_COMBINE_LAST_VALUE__

#include "signal_fwd.h"

//////////////////////////////////////////////////////////////////////////
/// 取最后一个返回值的合并器

namespace Bex
{
    namespace detail
    {
        template<typename T>
        class combine_last_value
        {
        public:
            typedef T result_type;

            template<typename InputIterator>
            T operator()(InputIterator first, InputIterator last) const
            {
                T value = T();
                while (first != last)
                {
                    try
                    {
                        value = *first;
                    }
                    catch(const boost_signals2::expired_slot &) {}
                    ++first;
                }
                return value;
            }
        };

        template<>
        class combine_last_value<void>
        {
        public:
            typedef void result_type;
            template<typename InputIterator>
            result_type operator()(InputIterator first, InputIterator last) const
            {
                while (first != last)
                {
                    try
                    {
                        *first;
                    }
                    catch(const boost_signals2::expired_slot &) {}
                    ++first;
                }
                return;
            }
        };
    }
}

#endif //__BEX_SIGNALS_COMBINE_LAST_VALUE__