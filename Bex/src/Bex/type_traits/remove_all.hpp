#ifndef __BEX_TYPE_TRAITS_REMOVE_ALL_HPP__
#define __BEX_TYPE_TRAITS_REMOVE_ALL_HPP__

#include <boost/type_traits/remove_all_extents.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_pointer.hpp>

namespace Bex
{
    //////////////////////////////////////////////////////////////////////////
    /// 迭代式剥掉类型的所有修饰
    namespace detail
    {
        template <typename T>
        struct remove_once
        {
            typedef typename boost::remove_all_extents<T>::type T1;
            typedef typename boost::remove_reference<T1>::type T2;
            typedef typename boost::remove_cv<T2>::type T3;
            typedef typename boost::remove_pointer<T3>::type T4;
            typedef T4 type;
        };

        template <typename T>
        struct has_wrapper
        {
            static const bool value = boost::is_array<T>::value ||
                boost::is_reference<T>::value ||
                boost::is_const<T>::value ||
                boost::is_volatile<T>::value ||
                boost::is_pointer<T>::value;
        };

        template <typename T>
        struct remove_all;

        template <typename T, bool has_wrap>
        struct remove_all_helper
        {
            typedef typename remove_all< typename remove_once<T>::type >::type type;
        };

        template <typename T>
        struct remove_all_helper<T, false>
        {
            typedef T type;
        };

        template <typename T>
        struct remove_all
            : remove_all_helper<T, has_wrapper<T>::value>
        {
        };
    } //namespace detail

    using detail::remove_all;

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_REMOVE_ALL_HPP__