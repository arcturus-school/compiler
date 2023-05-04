#include "ast.h"

int nid = 0;
int aid = 0;

AstNode* newAstSymbolNode(char* name, int lineno, int scope) {
    AstSymbolNode* s = (AstSymbolNode*)malloc(sizeof(AstSymbolNode));

    isOutOfMem(s);

    char* n = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    isOutOfMem(n);
    strcpy(n, name);

    s->nodeType = VARIABLE_NODE;  // 默认变量
    s->type     = UNDEF_TYPE;
    s->pointer  = 0;
    s->name     = n;
    s->value    = NULL;  // 默认没有值
    s->size     = 0;     // 默认不是数组
    s->scope    = scope;
    s->next     = NULL;
    s->id       = -1;  // 这个后面再赋值

    // 第一个结点主要用于标记声明变量的行数
    // 所以除了行数其他都是空的
    AstRefNode* ref = (AstRefNode*)newAstRefNode(s, lineno);
    s->lines        = ref;

    addToLink(s);
    addToLink(n);

    return (AstNode*)s;
}

AstNode* newAstRootNode(AstNode* next) {
    AstRootNode* node = malloc(sizeof(AstRootNode));

    node->next     = next;
    node->nodeType = ROOT_NODE;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstConstNode(DataTypes type, valueTypes value) {
    AstConstNode* node = (AstConstNode*)malloc(sizeof(AstConstNode));

    isOutOfMem(node);

    node->nodeType = CONST_NODE;
    node->dataType = type;
    node->next     = NULL;
    node->value    = value;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstIfNode(AstNode* branch) {
    AstIfNode* node = (AstIfNode*)malloc(sizeof(AstIfNode));

    isOutOfMem(node);

    node->nodeType = IF_STMT;
    node->next     = NULL;
    node->branch   = (AstIfBranchNode*)branch;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstIfBranchNode(AstNode* condition, AstNode* body) {
    AstIfBranchNode* node = (AstIfBranchNode*)malloc(sizeof(AstIfBranchNode));

    isOutOfMem(node);

    node->nodeType  = IF_BRANCH_STMT;
    node->next      = NULL;
    node->condition = condition;
    node->body      = body;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstForNode(AstNode* initialization, AstNode* condition, AstNode* end, AstNode* body) {
    AstForNode* node = (AstForNode*)malloc(sizeof(AstForNode));

    isOutOfMem(node);

    node->nodeType       = FOR_STMT;
    node->next           = NULL;
    node->initialization = initialization;
    node->condition      = condition;
    node->end            = end;
    node->body           = body;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstWhileNode(AstNode* condition, AstNode* body) {
    AstWhileNode* node = (AstWhileNode*)malloc(sizeof(AstWhileNode));

    isOutOfMem(node);

    node->nodeType  = WHILE_STMT;
    node->next      = NULL;
    node->condition = condition;
    node->body      = body;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstDeclNode(AstNode* s) {
    AstDeclNode* node = (AstDeclNode*)malloc(sizeof(AstDeclNode));

    isOutOfMem(node);

    node->nodeType = DECL_STMT;
    node->next     = NULL;
    node->variable = (AstSymbolNode*)s;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstRefNode(AstSymbolNode* s, int lineno) {
    AstRefNode* ref = (AstRefNode*)malloc(sizeof(AstRefNode));

    isOutOfMem(ref);

    ref->next     = NULL;
    ref->lines    = NULL;
    ref->variable = s;
    ref->lineno   = lineno;
    ref->nodeType = DIRECT_NDOE;  // 默认直接使用变量
    ref->indicate = NULL;

    addToLink(ref);

    return (AstNode*)ref;
}

AstNode* newAstContinueNode() {
    AstContinueNode* node = (AstContinueNode*)malloc(sizeof(AstContinueNode));

    isOutOfMem(node);

    node->nodeType = CONTINUE_STMT;
    node->next     = NULL;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstBreakNode() {
    AstBreakNode* node = (AstBreakNode*)malloc(sizeof(AstBreakNode));

    isOutOfMem(node);

    node->nodeType = BREAK_STMT;
    node->next     = NULL;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstAssignNode(AstNode* left, AstNode* right) {
    AstAssignNode* node = (AstAssignNode*)malloc(sizeof(AstAssignNode));

    isOutOfMem(node);

    node->nodeType = ASSIGN_EXP;
    node->next     = NULL;
    node->left     = left;
    node->right    = right;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstIncNode(AstRefNode* s, int isPrefix) {
    AstIncNode* node = (AstIncNode*)malloc(sizeof(AstIncNode));

    isOutOfMem(node);

    node->nodeType = INC_EXP;
    node->next     = NULL;
    node->variable = s;
    node->isPrefix = isPrefix;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstDecNode(AstRefNode* s, int isPrefix) {
    AstDecNode* node = (AstDecNode*)malloc(sizeof(AstDecNode));

    isOutOfMem(node);

    node->nodeType = DEC_EXP;
    node->next     = NULL;
    node->variable = s;
    node->isPrefix = isPrefix;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstArithmNode(ArithmOp op, AstNode* left, AstNode* right) {
    AstArithmNode* node = (AstArithmNode*)malloc(sizeof(AstArithmNode));

    isOutOfMem(node);

    node->nodeType = ARITHM_EXP;
    node->next     = NULL;
    node->op       = op;
    node->left     = left;
    node->right    = right;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstBoolNode(BoolOp op, AstNode* left, AstNode* right) {
    AstBoolNode* node = (AstBoolNode*)malloc(sizeof(AstBoolNode));

    isOutOfMem(node);

    node->nodeType = BOOL_EXP;
    node->next     = NULL;
    node->op       = op;
    node->left     = left;
    node->right    = right;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstRelNode(RelOp op, AstNode* left, AstNode* right) {
    AstRelNode* node = (AstRelNode*)malloc(sizeof(AstRelNode));

    isOutOfMem(node);

    node->nodeType = REL_EXP;
    node->next     = NULL;
    node->op       = op;
    node->left     = left;
    node->right    = right;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstEquNode(EquOp op, AstNode* left, AstNode* right) {
    AstEquNode* node = (AstEquNode*)malloc(sizeof(AstEquNode));

    isOutOfMem(node);

    node->nodeType = EQU_EXP;
    node->next     = NULL;
    node->op       = op;
    node->left     = left;
    node->right    = right;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstFuncCallNode(AstNode* function, AstNode* arguments) {
    AstFuncCallNode* node = (AstFuncCallNode*)malloc(sizeof(AstFuncCallNode));

    isOutOfMem(node);

    node->nodeType  = FUNC_CALL_EXP;
    node->next      = NULL;
    node->arguments = arguments;
    node->function  = (AstDeclNode*)function;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstFuncDeclNode(AstNode* function, AstNode* params, AstNode* body) {
    AstFuncDeclNode* node = (AstFuncDeclNode*)malloc(sizeof(AstFuncDeclNode));

    isOutOfMem(node);

    node->nodeType     = FUNC_DEF;
    node->function     = (AstSymbolNode*)function;
    node->params       = params;
    node->body         = body;
    node->next         = NULL;
    function->nodeType = FUNC_NODE;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* newAstReturnNode(DataTypes type, AstNode* exp) {
    AstReturnNode* node = (AstReturnNode*)malloc(sizeof(AstReturnNode));

    isOutOfMem(node);

    node->nodeType   = RETURN_NODE;
    node->next       = NULL;
    node->dataType   = type;
    node->expression = exp;

    addToLink(node);

    return (AstNode*)node;
}

AstNode* getLastSiblingNode(AstNode* node) {
    AstNode *t = node, *pre = NULL;

    while (t) {
        pre = t;
        t   = t->next;
    }

    // 不需要关心 return 后面的任何语句
    if (pre->nodeType == RETURN_NODE || pre->nodeType == BREAK_STMT || pre->nodeType == CONTINUE_STMT) {
        return NULL;
    }

    return pre;
}

void giveNodeType(AstNode* node, DataTypes type) {
    AstNode* t = node;

    while (t) {
        AstSymbolNode* temp = (AstSymbolNode*)((AstDeclNode*)t)->variable;
        temp->type          = type;

        if (temp->nodeType == VARIABLE_NODE) {
            temp->id = nid++;
        } else if (temp->nodeType == ARRAY_NODE) {
            temp->id = aid++;
        }

        t = t->next;
    }
}