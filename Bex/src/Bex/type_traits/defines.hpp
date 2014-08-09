#ifndef __BEX_TYPE_TRAITS_DEFINES_HPP__
#define __BEX_TYPE_TRAITS_DEFINES_HPP__

//////////////////////////////////////////////////////////////////////////
/// type_traits所需的基本定义
#include <Bex/config.hpp>

namespace Bex
{
    typedef char yes_type;
    typedef short no_type;

    struct array_type {};
    struct not_array_type {};

    template <typename T>
    T make();

    template <typename T>
    T& make_left_reference();

    template <typename T>
    T const& make_c_left_reference();

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    template <typename T>
    T&& make_right_reference();

    template <typename T>
    T const&& make_c_right_reference();
#endif

    template <typename T>
    T* make_pointer();

    template <typename T>
    T const* make_c_pointer();
    
} //namespace Bex

#endif //__BEX_TYPE_TRAITS_DEFINES_HPP__