#ifndef __AST_TYPES_H__
#define __AST_TYPES_H__

// 结点类型
typedef enum NodeTypes {
    UNKNOWN = -1,   // 未知类型结点
    ROOT_NODE,      // 根结点
    CONST_NODE,     // 常数据结点
    VARIABLE_NODE,  // 变量
    ARRAY_NODE,     // 数组
    FUNC_NODE,      // 函数
    FUNC_DEF,       // 函数定义结点
    RETURN_NODE,    // 函数返回值结点
} NodeTypes;

// 引用方式
typedef enum RefTypes {
    ARRAYITEM_NODE = 7,  // 数组方式(a[1])
    GETTER_NODE,         // 指针取值(*a)
    DIRECT_NDOE,         // 直接调用
    ADDRESS_NODE,        // 取地址(&a)
} RefTypes;

// 变量类型
typedef enum DataTypes {
    INT_TYPE,     // int
    DOUBLE_TYPE,  // double
    VOID_TYPE,    // void
    UNDEF_TYPE,   // undefined
    CHAR_TYPE,    // char
    STRING_TYPE,  // char*
} DataTypes;

// 语句类型
typedef enum StatementTypes {
    CONTINUE_STMT = 11,  // continue
    BREAK_STMT,          // break
    FOR_STMT,            // for
    WHILE_STMT,          // while
    IF_STMT,             // if
    IF_BRANCH_STMT,      // elif/else
    DECL_STMT,           // 变量声明
} StatementTypes;

// 表达式类型
typedef enum ExpressionTypes {
    FUNC_CALL_EXP = 18,  // 函数调用
    ASSIGN_EXP,          // =
    INC_EXP,             // ++
    DEC_EXP,             // --;
    ARITHM_EXP,          // + - * / %
    BOOL_EXP,            // &&, ||, !
    REL_EXP,             // >, <, >=, <=
    EQU_EXP,             // ==, !=
} ExpressionTypes;

// 算术运算
typedef enum ArithmOp {
    ADD,  // +
    SUB,  // -
    MUL,  // *
    MOD,  // %
    DIV,  // /
} ArithmOp;

// 逻辑运算
typedef enum BoolOp {
    OR,   // ||
    AND,  // &&
    NOT   // !
} BoolOp;

// 关系运算
typedef enum RelOp {
    GT,  // >
    LT,  // <
    GE,  // >=
    LE,  // <=
} RelOp;

typedef enum EquOp {
    EQ,  // ==
    NEQ  // !=
} EquOp;

#endif