#ifndef __BEX_TYPE_TRAITS_CLASS_INFO_HAS_OPERATOR__
#define __BEX_TYPE_TRAITS_CLASS_INFO_HAS_OPERATOR__

#include <Bex/type_traits/defines.hpp>
#include <Bex/utility/operators.h>
#include <Bex/config.hpp>
//////////////////////////////////////////////////////////////////////////
/// 类是否重载了指定操作符的成员函数

/* 当且仅当操作符是赋值操作符时, 识别的是非模板形式, 其余操作符均识别到模板形式.
    struct X
    {
        template <typename T>
        X& operator OPERATOR(T);
    };

    struct X
    {
        X& operator=(T);
    };
*/

namespace Bex
{

#ifdef _MSC_VER
# pragma warning(push)
#  pragma warning(disable: 4551)
#endif //_MSC_VER
    
#if !defined(BEX_SUPPORT_CXX11)

#define _BEX_TT_HAS_OPERATOR(traits_name, operator_name)                    \
    template <typename T>                                                   \
    struct traits_name                                                      \
    {                                                                       \
        template <bool>                                                     \
        class impl;                                                         \
                                                                            \
        template <typename U>                                               \
        static yes_type test(impl< ((&U::operator operator_name) && 0) >*); \
                                                                            \
        template <typename U>                                               \
        static no_type test(...);                                           \
                                                                            \
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type)); \
    }

    /// 数学计算
    _BEX_TT_HAS_OPERATOR(has_template_operator_add, BEX_OPERATOR_ADD);
    _BEX_TT_HAS_OPERATOR(has_template_operator_inc, BEX_OPERATOR_INC);
    _BEX_TT_HAS_OPERATOR(has_template_operator_add_assign, BEX_OPERATOR_ADD_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_sub, BEX_OPERATOR_SUB);
    _BEX_TT_HAS_OPERATOR(has_template_operator_dec, BEX_OPERATOR_DEC);
    _BEX_TT_HAS_OPERATOR(has_template_operator_sub_assign, BEX_OPERATOR_SUB_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_multi, BEX_OPERATOR_MULTI);
    _BEX_TT_HAS_OPERATOR(has_template_operator_multi_assign, BEX_OPERATOR_MULTI_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_div, BEX_OPERATOR_DIV);
    _BEX_TT_HAS_OPERATOR(has_template_operator_div_assign, BEX_OPERATOR_DIV_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_module, BEX_OPERATOR_MODULE);
    _BEX_TT_HAS_OPERATOR(has_template_operator_module_assign, BEX_OPERATOR_MODULE_ASSIGN);

    /// 二进制运算
    _BEX_TT_HAS_OPERATOR(has_template_operator_bit_and, BEX_OPERATOR_BIT_AND);
    _BEX_TT_HAS_OPERATOR(has_template_operator_bit_and_assign, BEX_OPERATOR_BIT_AND_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_bit_or, BEX_OPERATOR_BIT_OR);
    _BEX_TT_HAS_OPERATOR(has_template_operator_bit_or_assign, BEX_OPERATOR_BIT_OR_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_bit_not, BEX_OPERATOR_BIT_NOT);
    _BEX_TT_HAS_OPERATOR(has_template_operator_exclusive_or, BEX_OPERATOR_EXCLUSIVE_OR);
    _BEX_TT_HAS_OPERATOR(has_template_operator_exclusive_or_assign, BEX_OPERATOR_EXCLUSIVE_OR_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_left_shift, BEX_OPERATOR_LEFT_SHIFT);
    _BEX_TT_HAS_OPERATOR(has_template_operator_left_shift_assign, BEX_OPERATOR_LEFT_SHIFT_ASSIGN);
    _BEX_TT_HAS_OPERATOR(has_template_operator_right_shift, BEX_OPERATOR_RIGHT_SHIFT);
    _BEX_TT_HAS_OPERATOR(has_template_operator_right_shift_assign, BEX_OPERATOR_RIGHT_SHIFT_ASSIGN);

    /// 逻辑运算
    _BEX_TT_HAS_OPERATOR(has_template_operator_logical_not, BEX_OPERATOR_LOGICAL_NOT);
    _BEX_TT_HAS_OPERATOR(has_template_operator_logical_and, BEX_OPERATOR_LOGICAL_AND);
    _BEX_TT_HAS_OPERATOR(has_template_operator_logical_or, BEX_OPERATOR_LOGICAL_OR);

    /// 比较
    _BEX_TT_HAS_OPERATOR(has_template_operator_less, BEX_OPERATOR_LESS);
    _BEX_TT_HAS_OPERATOR(has_template_operator_less_or_equal, BEX_OPERATOR_LESS_OR_EQUAL);
    _BEX_TT_HAS_OPERATOR(has_template_operator_equal, BEX_OPERATOR_EQUAL);
    _BEX_TT_HAS_OPERATOR(has_template_operator_greater, BEX_OPERATOR_GREATER);
    _BEX_TT_HAS_OPERATOR(has_template_operator_greater_or_equal, BEX_OPERATOR_GREATER_OR_EQUAL);
    _BEX_TT_HAS_OPERATOR(has_template_operator_not_equal, BEX_OPERATOR_NOT_EQUAL);

    /// 其他运算符
    _BEX_TT_HAS_OPERATOR(has_template_operator_address_of, BEX_OPERATOR_ADDRESS_OF);
    _BEX_TT_HAS_OPERATOR(has_template_operator_invoke, BEX_OPERATOR_INVOKE);
    _BEX_TT_HAS_OPERATOR(has_template_operator_dereference, BEX_OPERATOR_DEREFERENCE);
    _BEX_TT_HAS_OPERATOR(has_template_operator_member_select, BEX_OPERATOR_MEMBER_SELECT);    
    _BEX_TT_HAS_OPERATOR(has_template_operator_subscript, BEX_OPERATOR_SUBSCRIPT);
    _BEX_TT_HAS_OPERATOR(has_template_operator_new, BEX_OPERATOR_NEW);
    _BEX_TT_HAS_OPERATOR(has_template_operator_delete, BEX_OPERATOR_DELETE);
    
    /// 赋值操作符
    _BEX_TT_HAS_OPERATOR(has_operator_assign, BEX_OPERATOR_ASSIGN);

    /// 逗号操作符
    template <typename T>
    struct has_template_operator_comma
    {
        template <bool>
        class impl;

        template <typename U>
        static yes_type test(impl< ((&U::operator ,) && 0) >*);

        template <typename U>
        static no_type test(...);

        static const bool value = (sizeof(test<T>(0)) == sizeof(yes_type));
    };

#undef _BEX_TT_HAS_OPERATOR

#else //!defined(BEX_SUPPORT_CXX11)

#define BEX_TT_HAS_BINARY_OPERATOR(has_name, Operator)                                  \
    template <typename T, typename Right = T>                                           \
    struct has_name                                                                     \
    {                                                                                   \
        template <typename U>                                                           \
        static no_type test(...);                                                       \
                                                                                        \
        template <typename U, typename D = decltype(make_left_reference<U>() Operator make<Right>()) > \
        static yes_type test(int*);                                                     \
                                                                                        \
        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));       \
    };

#define BEX_TT_HAS_UNARY_PREFIX_OPERATOR(has_name, Operator)                            \
    template <typename T>                                                               \
    struct has_name                                                                     \
    {                                                                                   \
        template <typename U>                                                           \
        static no_type test(...);                                                       \
                                                                                        \
        template <typename U, typename D = decltype(Operator make_left_reference<U>()) >\
        static yes_type test(int*);                                                     \
                                                                                        \
        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));       \
    };

#define BEX_TT_HAS_UNARY_POSTFIX_OPERATOR(has_name, Operator)                           \
    template <typename T>                                                               \
    struct has_name                                                                     \
    {                                                                                   \
        template <typename U>                                                           \
        static no_type test(...);                                                       \
                                                                                        \
        template <typename U, typename D = decltype(make_left_reference<U>() Operator) >\
        static yes_type test(int*);                                                     \
                                                                                        \
        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));       \
    };

#define BEX_TT_HAS_INVOKE_OPERATOR(has_name, Operator)                                                  \
    template <typename T, typename ... Args>                                                            \
    struct has_name                                                                                     \
    {                                                                                                   \
        template <typename U>                                                                           \
        static no_type test(...);                                                                       \
                                                                                                        \
        template <typename U, typename D = decltype(make<U>().operator Operator(make<Args>()...)) >     \
        static yes_type test(int*);                                                                     \
                                                                                                        \
        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));                       \
    };

    /// 一元运算符
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_unary_plus, BEX_OPERATOR_UNARY_PLUS)
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_unary_minus, BEX_OPERATOR_UNARY_NEGATION)
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_prefix_increment, BEX_OPERATOR_INC)
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_prefix_decrement, BEX_OPERATOR_DEC)
    BEX_TT_HAS_UNARY_POSTFIX_OPERATOR(can_postfix_increment, BEX_OPERATOR_INC)
    BEX_TT_HAS_UNARY_POSTFIX_OPERATOR(can_postfix_decrement, BEX_OPERATOR_DEC)

    BEX_TT_HAS_INVOKE_OPERATOR(has_unary_plus, BEX_OPERATOR_UNARY_PLUS)
    BEX_TT_HAS_INVOKE_OPERATOR(has_unary_minus, BEX_OPERATOR_UNARY_NEGATION)
    BEX_TT_HAS_INVOKE_OPERATOR(has_increment, BEX_OPERATOR_INC)
    BEX_TT_HAS_INVOKE_OPERATOR(has_decrement, BEX_OPERATOR_DEC)

    template <typename T, typename ... Args>
    using has_prefix_increment = has_increment<T>;

    template <typename T, typename ... Args>
    using has_postfix_increment = has_increment<T, int>;

    template <typename T, typename ... Args>
    using has_prefix_decrement = has_decrement<T>;

    template <typename T, typename ... Args>
    using has_postfix_decrement = has_decrement<T, int>;

    /// 数学计算
    BEX_TT_HAS_BINARY_OPERATOR(can_plus, BEX_OPERATOR_ADD)
    BEX_TT_HAS_BINARY_OPERATOR(can_plus_assign, BEX_OPERATOR_ADD_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_minus, BEX_OPERATOR_SUB)
    BEX_TT_HAS_BINARY_OPERATOR(can_minus_assign, BEX_OPERATOR_SUB_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_multiplication, BEX_OPERATOR_MULTI)
    BEX_TT_HAS_BINARY_OPERATOR(can_multiplication_assign, BEX_OPERATOR_MULTI_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_division, BEX_OPERATOR_DIV)
    BEX_TT_HAS_BINARY_OPERATOR(can_division_assign, BEX_OPERATOR_DIV_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_modulo, BEX_OPERATOR_MODULE)
    BEX_TT_HAS_BINARY_OPERATOR(can_modulo_assign, BEX_OPERATOR_MODULE_ASSIGN)

    BEX_TT_HAS_INVOKE_OPERATOR(has_plus, BEX_OPERATOR_ADD)
    BEX_TT_HAS_INVOKE_OPERATOR(has_plus_assign, BEX_OPERATOR_ADD_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_minus, BEX_OPERATOR_SUB)
    BEX_TT_HAS_INVOKE_OPERATOR(has_minus_assign, BEX_OPERATOR_SUB_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_multiplication, BEX_OPERATOR_MULTI)
    BEX_TT_HAS_INVOKE_OPERATOR(has_multiplication_assign, BEX_OPERATOR_MULTI_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_division, BEX_OPERATOR_DIV)
    BEX_TT_HAS_INVOKE_OPERATOR(has_division_assign, BEX_OPERATOR_DIV_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_modulo, BEX_OPERATOR_MODULE)
    BEX_TT_HAS_INVOKE_OPERATOR(has_modulo_assign, BEX_OPERATOR_MODULE_ASSIGN)
        
    /// 二进制运算
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_and, BEX_OPERATOR_BIT_AND)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_and_assign, BEX_OPERATOR_BIT_AND_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_or, BEX_OPERATOR_BIT_OR)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_or_assign, BEX_OPERATOR_BIT_OR_ASSIGN)
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_bitwise_not, BEX_OPERATOR_BIT_NOT)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_xor, BEX_OPERATOR_EXCLUSIVE_OR)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_xor_assign, BEX_OPERATOR_EXCLUSIVE_OR_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_left_shift, BEX_OPERATOR_LEFT_SHIFT)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_left_shift_assign, BEX_OPERATOR_LEFT_SHIFT_ASSIGN)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_right_shift, BEX_OPERATOR_RIGHT_SHIFT)
    BEX_TT_HAS_BINARY_OPERATOR(can_bitwise_right_shift_assign, BEX_OPERATOR_RIGHT_SHIFT_ASSIGN)

    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_and, BEX_OPERATOR_BIT_AND)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_and_assign, BEX_OPERATOR_BIT_AND_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_or, BEX_OPERATOR_BIT_OR)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_or_assign, BEX_OPERATOR_BIT_OR_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_not, BEX_OPERATOR_BIT_NOT)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_xor, BEX_OPERATOR_EXCLUSIVE_OR)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_xor_assign, BEX_OPERATOR_EXCLUSIVE_OR_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_left_shift, BEX_OPERATOR_LEFT_SHIFT)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_left_shift_assign, BEX_OPERATOR_LEFT_SHIFT_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_right_shift, BEX_OPERATOR_RIGHT_SHIFT)
    BEX_TT_HAS_INVOKE_OPERATOR(has_bitwise_right_shift_assign, BEX_OPERATOR_RIGHT_SHIFT_ASSIGN)

    /// 逻辑运算
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_logical_not, BEX_OPERATOR_LOGICAL_NOT)
    BEX_TT_HAS_BINARY_OPERATOR(can_logical_and, BEX_OPERATOR_LOGICAL_AND)
    BEX_TT_HAS_BINARY_OPERATOR(can_logical_or, BEX_OPERATOR_LOGICAL_OR)

    BEX_TT_HAS_INVOKE_OPERATOR(has_logical_not, BEX_OPERATOR_LOGICAL_NOT)
    BEX_TT_HAS_INVOKE_OPERATOR(has_logical_and, BEX_OPERATOR_LOGICAL_AND)
    BEX_TT_HAS_INVOKE_OPERATOR(has_logical_or, BEX_OPERATOR_LOGICAL_OR)

    /// 比较
    BEX_TT_HAS_BINARY_OPERATOR(can_less, BEX_OPERATOR_LESS)
    BEX_TT_HAS_BINARY_OPERATOR(can_less_or_equal, BEX_OPERATOR_LESS_OR_EQUAL)
    BEX_TT_HAS_BINARY_OPERATOR(can_equal, BEX_OPERATOR_EQUAL)
    BEX_TT_HAS_BINARY_OPERATOR(can_greater, BEX_OPERATOR_GREATER)
    BEX_TT_HAS_BINARY_OPERATOR(can_greater_or_equal, BEX_OPERATOR_GREATER_OR_EQUAL)
    BEX_TT_HAS_BINARY_OPERATOR(can_not_equal, BEX_OPERATOR_NOT_EQUAL)

    BEX_TT_HAS_INVOKE_OPERATOR(has_less, BEX_OPERATOR_LESS)
    BEX_TT_HAS_INVOKE_OPERATOR(has_less_or_equal, BEX_OPERATOR_LESS_OR_EQUAL)
    BEX_TT_HAS_INVOKE_OPERATOR(has_equal, BEX_OPERATOR_EQUAL)
    BEX_TT_HAS_INVOKE_OPERATOR(has_greater, BEX_OPERATOR_GREATER)
    BEX_TT_HAS_INVOKE_OPERATOR(has_greater_or_equal, BEX_OPERATOR_GREATER_OR_EQUAL)
    BEX_TT_HAS_INVOKE_OPERATOR(has_not_equal, BEX_OPERATOR_NOT_EQUAL)

    /// 其他运算符
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_address_of, BEX_OPERATOR_ADDRESS_OF)
    BEX_TT_HAS_UNARY_POSTFIX_OPERATOR(can_invoke, BEX_OPERATOR_INVOKE)
    BEX_TT_HAS_UNARY_PREFIX_OPERATOR(can_dereference, BEX_OPERATOR_DEREFERENCE)

    BEX_TT_HAS_INVOKE_OPERATOR(has_address_of, BEX_OPERATOR_ADDRESS_OF)
    BEX_TT_HAS_INVOKE_OPERATOR(has_invoke, BEX_OPERATOR_INVOKE)
    BEX_TT_HAS_INVOKE_OPERATOR(has_dereference, BEX_OPERATOR_DEREFERENCE)
    BEX_TT_HAS_INVOKE_OPERATOR(has_member_select, BEX_OPERATOR_MEMBER_SELECT)
    BEX_TT_HAS_INVOKE_OPERATOR(has_subscript, BEX_OPERATOR_SUBSCRIPT)
    BEX_TT_HAS_INVOKE_OPERATOR(has_new, BEX_OPERATOR_NEW)
    BEX_TT_HAS_INVOKE_OPERATOR(has_delete, BEX_OPERATOR_DELETE)

    /// 赋值操作符
    BEX_TT_HAS_BINARY_OPERATOR(can_assign, BEX_OPERATOR_ASSIGN)
    BEX_TT_HAS_INVOKE_OPERATOR(has_assign, BEX_OPERATOR_ASSIGN)

    /// 逗号操作符
    template <typename T, typename ... Args>
    struct has_comma
    {
        template <typename U>
        static no_type test(...);

        template <typename U, typename D = decltype(make<T>().operator,(make<Args>()...))>
        static yes_type test(int*);

        static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes_type));
    };

#endif //!defined(BEX_SUPPORT_CXX11)

#ifdef _MSC_VER
# pragma warning(pop)
#endif //_MSC_VER

} //namespace Bex

#endif //__BEX_TYPE_TRAITS_CLASS_INFO_HAS_OPERATOR__