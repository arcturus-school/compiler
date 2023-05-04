#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <graphviz/gvc.h>
#include "../ast.h"

#define MAX_KEY_LEN 20

// 获取图片 id
char* getKey();

// 抽象语法树转图片
void createImage(AstNode*, char*);

// 渲染结点
void createNode(Agraph_t*, Agnode_t*, AstNode*);

// 渲染根节点
void renderRootNode(Agraph_t*, AstRootNode*);

// 渲染函数定义
void renderFunctionDefNode(Agraph_t*, Agnode_t*, AstFuncDeclNode*);

// 渲染函数参数
void renderParameters(Agraph_t*, Agnode_t*, AstNode*);

// 渲染函数调用参数
void renderArguments(Agraph_t*, Agnode_t*, AstNode*);

// 渲染常量
void renderConstNode(Agraph_t*, Agnode_t*, AstConstNode*);

// 渲染函数调用
void renderFunctionCall(Agraph_t*, Agnode_t*, AstFuncCallNode*);

// 渲染简单语句
void renderSimpleNode(Agraph_t*, Agnode_t*, AstStatementNode*);

// 渲染语句块
void renderStatements(Agraph_t*, Agnode_t*, AstNode*, char*);

// 渲染 for 循环
void renderForNode(Agraph_t*, Agnode_t*, AstForNode*);

// 渲染 while 循环
void renderWhileNode(Agraph_t*, Agnode_t*, AstWhileNode*);

// 渲染 if
void renderIfNode(Agraph_t*, Agnode_t*, AstIfNode*);

// 渲染 if 分支
void renderIfBranchNode(Agraph_t*, Agnode_t*, AstIfBranchNode*);

// 渲染表达式
void renderExpressionNode(Agraph_t*, Agnode_t*, AstExpressionNode*);

// 变量声明
void renderVarDecl(Agraph_t*, Agnode_t*, AstDeclNode*);

// 变量使用
void renderVarRef(Agraph_t*, Agnode_t*, AstRefNode*);

// 渲染自增自减
void renderIncsNode(Agraph_t*, Agnode_t*, AstIncsNode*);

// 渲染返回值结点
void renderReturnNode(Agraph_t*, Agnode_t*, AstReturnNode*);

#endif