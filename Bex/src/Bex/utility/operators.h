#ifndef __BEX_UTILITY_OPERATORS_H__
#define __BEX_UTILITY_OPERATORS_H__

//////////////////////////////////////////////////////////////////////////
/// 所有运算符的宏定义

//////////////////////////////////////////////////////////////////////////
/// 可重载运算符

/// 数学计算
#define BEX_OPERATOR_ADD                        +       ///< 加法
#define BEX_OPERATOR_UNARY_PLUS                 +       ///< 正数符号
#define BEX_OPERATOR_INC                        ++      ///< 增1(有前置和后置两种)
#define BEX_OPERATOR_ADD_ASSIGN                 +=      ///< 加法 and 赋值
#define BEX_OPERATOR_SUB                        -       ///< 减法
#define BEX_OPERATOR_UNARY_NEGATION             -       ///< 负数符号
#define BEX_OPERATOR_DEC                        --      ///< 减1(有前置和后置两种)
#define BEX_OPERATOR_SUB_ASSIGN                 -=      ///< 减法 and 赋值
#define BEX_OPERATOR_MULTI                      *       ///< 乘法
#define BEX_OPERATOR_MULTI_ASSIGN               *=      ///< 乘法 and 赋值
#define BEX_OPERATOR_DIV                        /       ///< 除法
#define BEX_OPERATOR_DIV_ASSIGN                 /=      ///< 除法 and 赋值
#define BEX_OPERATOR_MODULE                     %       ///< 取模
#define BEX_OPERATOR_MODULE_ASSIGN              %=      ///< 取模 and 赋值

/// 二进制运算
#define BEX_OPERATOR_BIT_AND                    &       ///< 位与
#define BEX_OPERATOR_BIT_AND_ASSIGN             &=      ///< 位与 and 赋值
#define BEX_OPERATOR_BIT_OR                     |       ///< 位或
#define BEX_OPERATOR_BIT_OR_ASSIGN              |=      ///< 位或 and 赋值
#define BEX_OPERATOR_BIT_NOT                    ~       ///< 位反
#define BEX_OPERATOR_EXCLUSIVE_OR               ^       ///< 位异或
#define BEX_OPERATOR_EXCLUSIVE_OR_ASSIGN        ^=      ///< 位异或 and 赋值
#define BEX_OPERATOR_LEFT_SHIFT                 <<      ///< 按位左移
#define BEX_OPERATOR_LEFT_SHIFT_ASSIGN          <<=     ///< 按位左移 and 赋值
#define BEX_OPERATOR_RIGHT_SHIFT                >>      ///< 按位右移
#define BEX_OPERATOR_RIGHT_SHIFT_ASSIGN         >>=     ///< 按位右移 and 赋值

/// 逻辑运算
#define BEX_OPERATOR_LOGICAL_NOT                !       ///< 逻辑非
#define BEX_OPERATOR_LOGICAL_AND                &&      ///< 逻辑与
#define BEX_OPERATOR_LOGICAL_OR                 ||      ///< 逻辑或

/// 比较
#define BEX_OPERATOR_LESS                       <       ///< 小于
#define BEX_OPERATOR_LESS_OR_EQUAL              <=      ///< 小于or等于
#define BEX_OPERATOR_EQUAL                      ==      ///< 等于
#define BEX_OPERATOR_GREATER                    >       ///< 大于
#define BEX_OPERATOR_GREATER_OR_EQUAL           >=      ///< 大于or等于
#define BEX_OPERATOR_NOT_EQUAL                  !=      ///< 不等于

/// 其他运算符
#define BEX_OPERATOR_COMMA                      ,       ///< 逗号操作符
#define BEX_OPERATOR_ADDRESS_OF                 &       ///< 取地址操作符
#define BEX_OPERATOR_INVOKE                     ()      ///< 函数调用
#define BEX_OPERATOR_DEREFERENCE                *       ///< 解引用操作符
#define BEX_OPERATOR_MEMBER_SELECT              ->
#define BEX_OPERATOR_POINTER_MEMBER_SELECT      ->*     ///< 调用成员函数指针
#define BEX_OPERATOR_ASSIGN                     =       ///< 赋值
#define BEX_OPERATOR_SUBSCRIPT                  []      ///< 下标取值
#define BEX_OPERATOR_NEW                        new     ///< 申请内存
#define BEX_OPERATOR_DELETE                     delete  ///< 释放内存

//////////////////////////////////////////////////////////////////////////
/// 不可重载运算符
#define BEX_OPERATOR_MEMBER                     .
#define BEX_OPERATOR_POINT_MEMBER               .*
#define BEX_OPERATOR_SCOPE                      ::
#define BEX_OPERATOR_CONDITION                  ? :
#define BEX_OPERATOR_PERPROCESS_1               #
//#define BEX_OPERATOR_PERPROCESS_2             ##

#endif //__BEX_UTILITY_OPERATORS_H__