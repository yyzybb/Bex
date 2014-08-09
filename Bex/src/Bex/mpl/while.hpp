#ifndef __BEX_MPL_WHILE_HPP__
#define __BEX_MPL_WHILE_HPP__

#include <boost/mpl/if.hpp>
#include <boost/mpl/identity.hpp>
#include <Bex/type_traits/static_debug.hpp>
//////////////////////////////////////////////////////////////////////////
/// simple mpl while

namespace Bex { namespace mpl
{
    //////////////////////////////////////////////////////////////////////////
    /// mpl while
    // @Cond 循环控制条件函数
    // @Do   循环体执行函数
    // @T    循环数据
    // @Return T被循环处理的结果
    template <
        template<typename> class Cond,
        template<typename> class Do,
        typename T
    >
    struct while_
        : boost::mpl::if_<
                Cond<T>, 
                while_<Cond, Do, typename Do<T>::type>, 
                boost::mpl::identity<T>
            >::type
    {
        //BEX_STATIC_DEBUG_TYPEINFO(Cond<T>::value, T);
    };

} // namespace mpl
} // namespace Bex

#endif //__BEX_MPL_WHILE_HPP__