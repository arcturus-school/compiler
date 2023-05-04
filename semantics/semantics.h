#ifndef __SEMANTICS_H__
#define __SEMANTICS_H__

#include "../ast/ast.h"
#include "../utils/common.h"
#include "../utils/types.h"

extern int yylineno;

// 获取变量的类型
char* getTypeName(AstSymbolNode* node);

// 获取表达式类型
DataTypes getExpressionType(AstNode*);

// 检查所有兄弟结点类型, 不包括函数
void checkAstNodeType(AstNode*);

// 检查数组初始化的元素类型是否合规, 仅发出警告
void checkArrayElement(AstSymbolNode*);

// 判断变量所带数据的类型是否与声明时相符合, 并进行简单的类型转换
void checkSymbol(AstSymbolNode*);

// 判断数组声明是数组大小的情况
void checkArraySize(AstSymbolNode*, AstNode*, int);

// 判断数组调用时索引类型
void checkIndexType(AstRefNode*, AstNode*);

// 判断表达式和变量的类型是否匹配
void checkAssign(AstNode*, AstNode*);

// 判断函数返回值类型
void checkFunctionReturn(AstFuncDeclNode*, AstReturnNode*);

// 封装一些错误警告输出

// 类型不符警告
void initTypeWarning(char*, char*);

// 赋值时类型检查
void assignTypeWarning(char*, char*);

// 函数返回值类型
void retTypeWarning(char*, char*);

// 重复声明问题
void multiDeclareError(int, char*);

// 数组索引不是整型
void isNotInteger();

// 数组大小缺失
void sizeIsMissing(char*);

// 数组索引缺失
void indexIsMissing(char*);

// 变量未声明
void isUndeclared(int, char*);

// 无效操作
void opError(char*, char*, char*);

// 数组索引不能为空
void indexIsNull();

// 数组大小不能为负数
void sizeIsNegative(char*);

// 变量可能未初始化
void varMayNotBeInit();

// 数组初始化元素个数多于声明时个数
void excessElement();

// 赋值无效
void assignInvalid();

#endif