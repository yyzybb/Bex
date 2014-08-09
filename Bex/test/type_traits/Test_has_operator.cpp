#include "TestPCH.h"
#include <Bex/type_traits/class_info.hpp>
#include <Bex/config.hpp>
using namespace Bex;

#define TTT template <typename T>

//////////////////////////////////////////////////////////////////////////
/// 重载操作符(模板式, 二元, 后置)
struct TemplateTwoPostfix
{
    typedef TemplateTwoPostfix this_type;
    TTT TemplateTwoPostfix operator BEX_OPERATOR_ADD(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_INC(int);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_ADD_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_SUB(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_DEC(int);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_SUB_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_MULTI(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_MULTI_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_DIV(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_DIV_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_MODULE(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_MODULE_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_BIT_AND(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_BIT_AND_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_BIT_OR(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_BIT_OR_ASSIGN(T);
    TTT TemplateTwoPostfix operator BEX_OPERATOR_EXCLUSIVE_OR(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_EXCLUSIVE_OR_ASSIGN(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_LEFT_SHIFT(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_LEFT_SHIFT_ASSIGN(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_RIGHT_SHIFT(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_RIGHT_SHIFT_ASSIGN(T);
    TTT bool operator BEX_OPERATOR_LOGICAL_AND(T);
    TTT bool operator BEX_OPERATOR_LOGICAL_OR(T);
    TTT bool operator BEX_OPERATOR_LESS(T);
    TTT bool operator BEX_OPERATOR_LESS_OR_EQUAL(T);
    TTT bool operator BEX_OPERATOR_EQUAL(T);
    TTT bool operator BEX_OPERATOR_GREATER(T);
    TTT bool operator BEX_OPERATOR_GREATER_OR_EQUAL(T);
    TTT bool operator BEX_OPERATOR_NOT_EQUAL(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_COMMA(T);
    TTT TemplateTwoPostfix& operator BEX_OPERATOR_ASSIGN(T);
    TTT int& operator BEX_OPERATOR_SUBSCRIPT(int);
#if defined(_MSC_VER)
    TTT void* operator BEX_OPERATOR_NEW(size_t);
    TTT void operator BEX_OPERATOR_DELETE(void*);
#endif //defined(_MSC_VER)
};

/// 重载操作符(普通, 二元, 后置)
struct NormalTwoPostfix
{
    typedef NormalTwoPostfix this_type;
    NormalTwoPostfix operator BEX_OPERATOR_ADD(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_INC(int);
    NormalTwoPostfix& operator BEX_OPERATOR_ADD_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_SUB(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_DEC(int);
    NormalTwoPostfix& operator BEX_OPERATOR_SUB_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_MULTI(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_MULTI_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_DIV(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_DIV_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_MODULE(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_MODULE_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_BIT_AND(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_BIT_AND_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_BIT_OR(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_BIT_OR_ASSIGN(this_type);
    NormalTwoPostfix operator BEX_OPERATOR_EXCLUSIVE_OR(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_EXCLUSIVE_OR_ASSIGN(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_LEFT_SHIFT(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_LEFT_SHIFT_ASSIGN(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_RIGHT_SHIFT(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_RIGHT_SHIFT_ASSIGN(this_type);
    bool operator BEX_OPERATOR_LOGICAL_AND(this_type);
    bool operator BEX_OPERATOR_LOGICAL_OR(this_type);
    bool operator BEX_OPERATOR_LESS(this_type);
    bool operator BEX_OPERATOR_LESS_OR_EQUAL(this_type);
    bool operator BEX_OPERATOR_EQUAL(this_type);
    bool operator BEX_OPERATOR_GREATER(this_type);
    bool operator BEX_OPERATOR_GREATER_OR_EQUAL(this_type);
    bool operator BEX_OPERATOR_NOT_EQUAL(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_COMMA(this_type);
    NormalTwoPostfix& operator BEX_OPERATOR_ASSIGN(this_type);
    this_type& operator BEX_OPERATOR_SUBSCRIPT(int);
#if defined(_MSC_VER)
    void* operator BEX_OPERATOR_NEW(size_t);
    void operator BEX_OPERATOR_DELETE(void*);
#endif //defined(_MSC_VER)
};

/// 重载操作符(模板式, 一元, 前置)
struct TemplateUnaryPrefix
{
    TTT TemplateUnaryPrefix operator BEX_OPERATOR_UNARY_PLUS();
    TTT TemplateUnaryPrefix& operator BEX_OPERATOR_INC();
    TTT TemplateUnaryPrefix operator BEX_OPERATOR_UNARY_NEGATION();
    TTT TemplateUnaryPrefix& operator BEX_OPERATOR_DEC();
    TTT int& operator BEX_OPERATOR_DEREFERENCE();
    TTT int* operator BEX_OPERATOR_MEMBER_SELECT();
    TTT TemplateUnaryPrefix* operator BEX_OPERATOR_ADDRESS_OF();
    TTT TemplateUnaryPrefix operator BEX_OPERATOR_BIT_NOT();
    TTT bool operator BEX_OPERATOR_LOGICAL_NOT();
    TTT void operator BEX_OPERATOR_INVOKE();
};

/// 重载操作符(普通, 一元, 前置)
struct NormalUnaryPrefix
{
    NormalUnaryPrefix operator BEX_OPERATOR_UNARY_PLUS();
    NormalUnaryPrefix& operator BEX_OPERATOR_INC();
    NormalUnaryPrefix operator BEX_OPERATOR_UNARY_NEGATION();
    NormalUnaryPrefix& operator BEX_OPERATOR_DEC();
    int& operator BEX_OPERATOR_DEREFERENCE();
    int* operator BEX_OPERATOR_MEMBER_SELECT();
    NormalUnaryPrefix* operator BEX_OPERATOR_ADDRESS_OF();
    NormalUnaryPrefix operator BEX_OPERATOR_BIT_NOT();
    bool operator BEX_OPERATOR_LOGICAL_NOT();
    void operator BEX_OPERATOR_INVOKE();
};

class Empty {};
typedef int IntervalType;

#define CHECK_HAS_OPERATOR(traits_name, vA, vB, vC, vD, vEmpty, vIT)    \
    BOOST_CHECK(traits_name<TemplateTwoPostfix>::value == vA);          \
    BOOST_CHECK(traits_name<NormalTwoPostfix>::value == vB);            \
    BOOST_CHECK(traits_name<TemplateUnaryPrefix>::value == vC);         \
    BOOST_CHECK(traits_name<NormalUnaryPrefix>::value == vD);           \
    BOOST_CHECK(traits_name<Empty>::value == vEmpty);                   \
    BOOST_CHECK(traits_name<IntervalType>::value == vIT);

#define CHECK_HAS_OPERATOR_TWO(traits_name, vA, vB, vC, vD, vEmpty, vIT)              \
    BOOST_CHECK((traits_name<TemplateTwoPostfix, TemplateTwoPostfix>::value == vA));  \
    BOOST_CHECK((traits_name<NormalTwoPostfix, NormalTwoPostfix>::value == vB));      \
    BOOST_CHECK((traits_name<TemplateUnaryPrefix, TemplateUnaryPrefix>::value == vC));\
    BOOST_CHECK((traits_name<NormalUnaryPrefix, NormalUnaryPrefix>::value == vD));    \
    BOOST_CHECK((traits_name<Empty, Empty>::value == vEmpty));                        \
    BOOST_CHECK((traits_name<IntervalType, IntervalType>::value == vIT));

#define CHECK_HAS_OPERATOR_RIGHT(traits_name, R, vA, vB, vC, vD, vEmpty, vIT)       \
    BOOST_CHECK((traits_name<TemplateTwoPostfix, R>::value == vA));                 \
    BOOST_CHECK((traits_name<NormalTwoPostfix, R>::value == vB));                   \
    BOOST_CHECK((traits_name<TemplateUnaryPrefix, R>::value == vC));                \
    BOOST_CHECK((traits_name<NormalUnaryPrefix, R>::value == vD));                  \
    BOOST_CHECK((traits_name<Empty, R>::value == vEmpty));                          \
    BOOST_CHECK((traits_name<IntervalType, R>::value == vIT));

BOOST_AUTO_TEST_SUITE(s_has_operator)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_has_operator)
{
#if !defined(BEX_SUPPORT_CXX11)

    XDump("开始测试 has_operator. C++98");

    CHECK_HAS_OPERATOR(has_template_operator_add, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_inc, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_add_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_sub, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_dec, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_sub_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_multi, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_multi_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_div, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_div_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_module, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_module_assign, 1, 0, 0, 0, 0, 0);

    CHECK_HAS_OPERATOR(has_template_operator_bit_and, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_bit_and_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_bit_or, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_bit_or_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_bit_not, 0, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_exclusive_or, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_exclusive_or_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_left_shift, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_left_shift_assign, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_right_shift, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_right_shift_assign, 1, 0, 0, 0, 0, 0);

    CHECK_HAS_OPERATOR(has_template_operator_logical_not, 0, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_logical_and, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_logical_or, 1, 0, 0, 0, 0, 0);

    CHECK_HAS_OPERATOR(has_template_operator_less, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_less_or_equal, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_equal, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_greater, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_greater_or_equal, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_not_equal, 1, 0, 0, 0, 0, 0);


    CHECK_HAS_OPERATOR(has_template_operator_address_of, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_invoke, 0, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_dereference, 1, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_member_select, 0, 0, 1, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_subscript, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_new, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_delete, 1, 0, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_template_operator_comma, 1, 0, 0, 0, 0, 0);

    CHECK_HAS_OPERATOR(has_operator_assign, 0, 1, 0, 0, 0, 0);

#else //!defined(BEX_SUPPORT_CXX11)

    XDump("开始测试 has_operator. C++11");

// <TemplateTwoPostfix> <NormalTwoPostfix> <TemplateUnaryPrefix> <NormalUnaryPrefix> <Empty> <int>

    /// 一元运算符
    CHECK_HAS_OPERATOR(can_unary_plus,              0, 0, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR(can_unary_minus,             0, 0, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR(can_prefix_increment,        0, 0, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR(can_prefix_decrement,        0, 0, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR(can_postfix_increment,       0, 1, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR(can_postfix_decrement,       0, 1, 0, 1, 0, 1);

    CHECK_HAS_OPERATOR(has_unary_plus,              0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR(has_unary_minus,             0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR(has_prefix_increment,        0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR(has_prefix_decrement,        0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR(has_postfix_increment,       0, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR(has_postfix_decrement,       0, 1, 0, 0, 0, 0);

    /// 数学计算
    CHECK_HAS_OPERATOR_TWO(can_plus,                    1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_plus_assign,             1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_minus,                   1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_minus_assign,            1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_multiplication,          1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_multiplication_assign,   1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_division,                1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_division_assign,         1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_modulo,                  1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_modulo_assign,           1, 1, 0, 0, 0, 1);

    CHECK_HAS_OPERATOR_TWO(has_plus,                    1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_plus_assign,             1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_minus,                   1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_minus_assign,            1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_multiplication,          1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_multiplication_assign,   1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_division,                1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_division_assign,         1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_modulo,                  1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_modulo_assign,           1, 1, 0, 0, 0, 0);

    /// 二进制运算
    CHECK_HAS_OPERATOR_TWO(can_bitwise_and,                 1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_and_assign,          1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_or,                  1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_or_assign,           1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR    (can_bitwise_not,                 0, 0, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_xor,                 1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_xor_assign,          1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_left_shift,          1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_left_shift_assign,   1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_right_shift,         1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_bitwise_right_shift_assign,  1, 1, 0, 0, 0, 1);

    CHECK_HAS_OPERATOR_TWO(has_bitwise_and,                 1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_and_assign,          1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_or,                  1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_or_assign,           1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR    (has_bitwise_not,                 0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_xor,                 1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_xor_assign,          1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_left_shift,          1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_left_shift_assign,   1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_right_shift,         1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_bitwise_right_shift_assign,  1, 1, 0, 0, 0, 0);

    /// 逻辑运算
    CHECK_HAS_OPERATOR    (can_logical_not,             0, 0, 0, 1, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_logical_and,             1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_logical_or,              1, 1, 0, 0, 0, 1);

    CHECK_HAS_OPERATOR    (has_logical_not,             0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_logical_and,             1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_logical_or,              1, 1, 0, 0, 0, 0);

    /// 比较
    CHECK_HAS_OPERATOR_TWO(can_less,                    1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_less_or_equal,           1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_equal,                   1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_greater,                 1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_greater_or_equal,        1, 1, 0, 0, 0, 1);
    CHECK_HAS_OPERATOR_TWO(can_not_equal,               1, 1, 0, 0, 0, 1);

    CHECK_HAS_OPERATOR_TWO(has_less,                    1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_less_or_equal,           1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_equal,                   1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_greater,                 1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_greater_or_equal,        1, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_TWO(has_not_equal,               1, 1, 0, 0, 0, 0);

    /// 其他运算符
    CHECK_HAS_OPERATOR(can_address_of,                  1, 1, 1, 1, 1, 1);
    CHECK_HAS_OPERATOR(can_invoke,                      0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR(can_dereference,                 0, 0, 0, 1, 0, 0);

    CHECK_HAS_OPERATOR    (has_address_of,              0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR    (has_invoke,                  0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR    (has_dereference,             0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR    (has_member_select,           0, 0, 0, 1, 0, 0);
    CHECK_HAS_OPERATOR_RIGHT(has_subscript, int,        0, 1, 0, 0, 0, 0);
#if defined(_MSC_VER)
    CHECK_HAS_OPERATOR_RIGHT(has_new, std::size_t,      0, 1, 0, 0, 0, 0);
    CHECK_HAS_OPERATOR_RIGHT(has_delete, void*,         0, 1, 0, 0, 0, 0);
#endif //defined(_MSC_VER)

    /// 赋值操作符
    CHECK_HAS_OPERATOR_TWO(can_assign,                  1, 1, 1, 1, 1, 1);
    CHECK_HAS_OPERATOR_TWO(has_assign,                  1, 1, 1, 1, 1, 0);


#endif //!defined(BEX_SUPPORT_CXX11)

    XDump("结束测试 has_operator");
}

BOOST_AUTO_TEST_SUITE_END()