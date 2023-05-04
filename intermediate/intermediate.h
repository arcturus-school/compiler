#ifndef __INTERMEDIATE_H__
#define __INTERMEDIATE_H__

#include "../ast/ast.h"
#include "../utils/common.h"

#define LABEL_LEN 20

// 四元式
typedef struct Four {
    char* loc;     // 地址
    char* op;      // 运算符
    char* arg1;    // 操作数一
    char* arg2;    // 操作数二
    char* result;  // 运算结果
} Four;

// 生成四元式
Four* gen(char*, char*, char*, char*, char*);

typedef struct FourArray {
    int    capacity;
    int    top;
    Four** list;
} FourArray;

// 初始化四元式栈
FourArray* newFourArray();

// 重新分配四元式栈大小
void resize(FourArray*);

// 四元式入栈
void push(FourArray*, Four*);

// 四元式出栈
Four* pop(FourArray*);

// 栈的一些清理工作
void clean(FourArray*);

// 初始化文件流
void initFilePointer(char*);

// 关闭文件流
void closeFilePointer();

// 输出中间代码到文件中
void writeToFile();

// 变量是否使用过
int isUsed(AstSymbolNode*);

// 生成中间代码
void createIntermediateCode(AstNode*, char*);

// 获取类型占用内存数
int getSize(AstSymbolNode*);

// 条件判断
Four* createConditionIntermediateCode(AstExpressionNode*);

// 生成数组偏移量
char* createIndicateIntermediateCode(AstNode*, int);

// 生成函数调用的中间代码
char* createFuncCallIntermediateCode(AstFuncCallNode*);

// 生成立即数的中间代码
char* createConstIntermediateCode(AstConstNode*);

// 根据结点类型创建对应中间代码
void createCodeFromRoot(AstRootNode*);

// 生成函数声明的中间代码
void createFuncDeclIntermediateCode(AstFuncDeclNode*);

// 获取标签, such as getLabel("temp", 1); ==> "temp1"
char* getLabel(char*, ...);

// 生成语句块的中间代码
void createBodyIntermediateCode(AstNode*);

// 生成 return 语句的中间代码
void createReturnIntermediateCode(AstReturnNode*);

// 生成表达式的中间代码, 并返回最后一个变量名
char* createExpressionIntermediateCode(AstNode* node);

// 生成 for 语句的中间代码
void createForIntermediateCode(AstForNode*);

// 生成 while 语句的中间代码
void createWhileIntermediateCode(AstWhileNode*);

// 变量声明语句
void createVarDeclIntermediateCode(AstDeclNode*);

// 自增自减
char* createIncsIntermediateCode(AstIncsNode*);

// 赋值语句
char* createAssignIntermediateCode(AstAssignNode*);

// 分支语句
void createIfIntermediateCode(AstIfNode*);

#endif