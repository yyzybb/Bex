#ifndef __BEX_TYPE_TRAITS_STATIC_DEBUG_HPP__
#define __BEX_TYPE_TRAITS_STATIC_DEBUG_HPP__

//////////////////////////////////////////////////////////////////////////
/// 编译期输出调试信息 
//  @preparing

namespace Bex
{
    template <char, typename = int>
    struct static_debuger_integral {};

    template <bool, typename T>
    struct msvc12_static_debuger {};

    template <typename T>
    struct msvc12_static_debuger<false, T>
    {
    private:
        // C4244: 'initializing' : conversion from 'double' to 'const int', possible loss of data
        static const int i = 1.0;

        // C4309: 'specialization' : truncation of constant value
        typedef static_debuger_integral<127 + sizeof(T), T> type_;
    };

} // namespace Bex

#if defined(_MSC_VER) && _MSC_VER == 1800
# define BEX_STATIC_DEBUG_TYPEINFO(static_bool, type) \
    typedef ::Bex::static_debuger_integral<sizeof(::Bex::msvc12_static_debuger<static_bool, type>)> \
        static_debuger_;
#else
# define BEX_STATIC_DEBUG_TYPEINFO(static_bool, type)
#endif

#endif //__BEX_TYPE_TRAITS_STATIC_DEBUG_HPP__