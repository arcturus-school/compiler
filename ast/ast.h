#ifndef __ASTNODE_H__
#define __ASTNODE_H__

#include "../utils/common.h"
#include "../utils/types.h"

extern int nid;  // 变量 id
extern int aid;  // 数组 id

typedef struct AstNode {
    struct AstNode* next;
    enum NodeTypes  nodeType;
} AstNode, AstRootNode;

// 创建根节点
AstNode* newAstRootNode(AstNode*);

// 当前支持的数据类型
typedef union valueTypes {
    int    ival;
    double dval;
    char   cval;
    char*  sval;
} valueTypes;

typedef struct AstConstNode {
    AstNode*       next;  // 数组的话有用
    enum NodeTypes nodeType;
    enum DataTypes dataType;
    valueTypes     value;
} AstConstNode;

// 创建常数据结点
AstNode* newAstConstNode(DataTypes, valueTypes);

typedef struct AstSymbolNode AstSymbolNode;
typedef struct AstRefNode    AstRefNode;

struct AstRefNode {
    AstNode*       next;
    enum RefTypes  nodeType;
    AstSymbolNode* variable;
    int            lineno;    // 使用变量时所在行
    AstRefNode*    lines;     // 变量的引用链
    AstNode*       indicate;  // 如果是数组则表示索引, 如果是取值表示多少个 *
};

// 创建变量使用方式结点
AstNode* newAstRefNode(AstSymbolNode*, int);

struct AstSymbolNode {
    struct AstNode* next;
    enum NodeTypes  nodeType;  // 结点类型
    enum DataTypes  type;      // 变量类型或者函数返回值类型
    AstNode*        value;     // 数据, 如果是数组的话是一个链表
    AstRefNode*     lines;     // 引用链
    char*           name;      // 变量名/函数名/数组名
    int             scope;     // 变量所处的作用域
    int             pointer;   // 是否是指针
    int             size;      // 数组长度
    int             id;        // 变量 id
};

// 创建变量结点
AstNode* newAstSymbolNode(char*, int, int);

typedef struct AstStatementNode {
    AstNode*            next;
    enum StatementTypes nodeType;
} AstStatementNode, AstContinueNode, AstBreakNode;

// 创建 continue 结点
AstNode* newAstContinueNode();

// 创建 break 结点
AstNode* newAstBreakNode();

typedef struct AstIfBranchNode AstIfBranchNode;

typedef struct AstIfNode {
    AstNode*            next;
    enum StatementTypes nodeType;
    AstIfBranchNode*    branch;
} AstIfNode;

// 创建 if 语句结点
AstNode* newAstIfNode(AstNode*);

struct AstIfBranchNode {
    AstNode*            next;
    enum StatementTypes nodeType;
    AstNode*            condition;
    AstNode*            body;
};

// 创建 if 语句分支结点
AstNode* newAstIfBranchNode(AstNode*, AstNode*);

typedef struct AstForNode {
    AstNode*            next;
    enum StatementTypes nodeType;
    AstNode*            initialization;
    AstNode*            condition;
    AstNode*            end;
    AstNode*            body;
} AstForNode;

// 创建 for 循环结点
AstNode* newAstForNode(AstNode*, AstNode*, AstNode*, AstNode*);

typedef struct AstWhileNode {
    AstNode*            next;
    enum StatementTypes nodeType;
    AstNode*            condition;
    AstNode*            body;
} AstWhileNode;

// 创建 while 循环语句
AstNode* newAstWhileNode(AstNode*, AstNode*);

typedef struct AstDeclNode {
    AstNode*            next;
    enum StatementTypes nodeType;
    AstSymbolNode*      variable;  // 变量名
} AstDeclNode;

// 创建变量声明语句
AstNode* newAstDeclNode(AstNode*);

// 给予所有兄弟相同类型
void giveNodeType(AstNode*, DataTypes);

// 表达式结点, 赋值语句结点
typedef struct AstExpressionNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstNode*             left;
    AstNode*             right;
} AstExpressionNode, AstAssignNode;

// 创建赋值语句结点
AstNode* newAstAssignNode(AstNode*, AstNode*);

typedef struct AstIncNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstRefNode*          variable;
    int                  isPrefix;
} AstIncsNode, AstIncNode, AstDecNode;

// 创建自增结点
AstNode* newAstIncNode(AstRefNode*, int);

// 创建自减结点
AstNode* newAstDecNode(AstRefNode*, int);

typedef struct AstArithmNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstNode*             left;
    AstNode*             right;
    enum ArithmOp        op;
} AstArithmNode;

// 创建算术运算符结点
AstNode* newAstArithmNode(ArithmOp, AstNode*, AstNode*);

typedef struct AstBoolNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstNode*             left;
    AstNode*             right;
    enum BoolOp          op;
} AstBoolNode;

// 创建布尔运算符结点
AstNode* newAstBoolNode(BoolOp, AstNode*, AstNode*);

typedef struct AstRelNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstNode*             left;
    AstNode*             right;
    enum RelOp           op;
} AstRelNode;

// 创建关系运算符结点
AstNode* newAstRelNode(RelOp, AstNode*, AstNode*);

typedef struct AstEquNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstNode*             left;
    AstNode*             right;
    enum EquOp           op;
} AstEquNode;

// 创建关系运算符结点
AstNode* newAstEquNode(EquOp, AstNode*, AstNode*);

typedef struct AstFuncCallNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    AstDeclNode*         function;
    AstNode*             arguments;
} AstFuncCallNode;

// 创建函数调用结点
AstNode* newAstFuncCallNode(AstNode*, AstNode*);

typedef struct AstFuncDeclNode {
    AstNode*       next;
    enum NodeTypes nodeType;
    AstSymbolNode* function;  // 函数被当作一个变量
    AstNode*       params;    // 参数链表
    AstNode*       body;      // 函数体
} AstFuncDeclNode;

// 创建函数声明结点
AstNode* newAstFuncDeclNode(AstNode*, AstNode*, AstNode*);

typedef struct AstReturnNode {
    AstNode*             next;
    enum ExpressionTypes nodeType;
    DataTypes            dataType;
    AstNode*             expression;
} AstReturnNode;

// 创建 return 结点
AstNode* newAstReturnNode(DataTypes, AstNode*);

// 获取最后一个兄弟元素
AstNode* getLastSiblingNode(AstNode*);

#include "./graph/graph.h"

#endif