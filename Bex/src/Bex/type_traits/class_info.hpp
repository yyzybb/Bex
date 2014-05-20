#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HPP__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HPP__

//////////////////////////////////////////////////////////////////////////
/// class_info 用于识别类的信息
#include <Bex/type_traits/class_info/has_const_int.hpp>
#include <Bex/type_traits/class_info/has_member_data.hpp>
#include <Bex/type_traits/class_info/has_member_function.hpp>
#include <Bex/type_traits/class_info/has_static_member_data.hpp>
#include <Bex/type_traits/class_info/has_static_member_function.hpp>
#include <Bex/type_traits/class_info/has_template_function.hpp>
#include <Bex/type_traits/class_info/has_template_type.hpp>
#include <Bex/type_traits/class_info/has_type.hpp>
#include <Bex/type_traits/class_info/has_operator.hpp>
//////////////////////////////////////////////////////////////////////////

//struct X
//{
//    int member_data;                                ///< OK! 完美
//    static int static_member_data;                  ///< OK! 完美
//    void member_function(int);                      ///< OK! 完美
//    static int static_member_function(double);      ///< OK! 完美
//
//    /// typedef 和 自定义类型暂时无法细致区分
//    typedef int _typedef;           ///< OK!
//    struct declare_struct;          ///< OK!
//
//    template <typename T>
//    struct declare_template_struct; ///< 需指定参数列表, 实现参数可变后可以达到完美
//
//    template <typename T>
//    void member_template_function();    ///< 与static相同, 无法识别函数签名
//    template <typename T>
//    static void static_member_template_function(); ///< 与static相同, 无法识别函数签名
//
//    static const int init_static_const_int_member_data = 0; ///< OK! 与枚举相同
//    enum { enum_number, }; ///< OK! 与static const int相同
//};

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HPP__