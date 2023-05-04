#include "graph.h"

int gid = 0;  // 图结点编号

char* getKey() {
    char* key = (char*)malloc(sizeof(char) * (MAX_KEY_LEN + 6));
    addToLink(key);
    sprintf(key, "graph_%d", gid++);
    return key;
}

void renderRootNode(Agraph_t* g, AstRootNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", "root");

    AstNode* t = node->next;

    while (t) {
        createNode(g, r, t);
        t = t->next;
    }
}

void renderFunctionDefNode(Agraph_t* g, Agnode_t* parent, AstFuncDeclNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", node->function->name);
    agedge(g, parent, r, 0, 1);

    renderParameters(g, r, node->params);

    AstNode* b = node->body;
    while (b) {
        // 渲染所有语句
        createNode(g, r, b);
        b = b->next;
    }
}

void renderParameters(Agraph_t* g, Agnode_t* parent, AstNode* node) {
    if (node) {
        // 如果参数存在则进行绘制
        char* key = getKey();

        Agnode_t* params = agnode(g, key, 1);
        agset(params, "label", "parameters");
        agedge(g, parent, params, 0, 1);

        while (node) {
            createNode(g, params, node);
            node = node->next;
        }
    }
}

void renderConstNode(Agraph_t* g, Agnode_t* parent, AstConstNode* node) {
    char* key = getKey();

    char* label = (char*)malloc(sizeof(char) * 20);

    switch (node->dataType) {
        case INT_TYPE: {
            sprintf(label, "%d", node->value.ival);
            break;
        }
        case DOUBLE_TYPE: {
            sprintf(label, "%g", node->value.dval);
            break;
        }
        case CHAR_TYPE: {
            sprintf(label, "'%c'", node->value.cval);
            break;
        }
        case STRING_TYPE: {
            sprintf(label, "%s", node->value.sval);
            break;
        }
    }

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", label);
    agedge(g, parent, r, 0, 1);

    free(label);
}

void renderFunctionCall(Agraph_t* g, Agnode_t* parent, AstFuncCallNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", node->function->variable->name);
    agedge(g, parent, r, 0, 1);
    renderArguments(g, r, node->arguments);
}

void renderArguments(Agraph_t* g, Agnode_t* parent, AstNode* node) {
    if (node) {
        char* key = getKey();

        Agnode_t* args = agnode(g, key, 1);
        agset(args, "label", "arguments");
        agedge(g, parent, args, 0, 1);

        while (node) {
            createNode(g, args, node);
            node = node->next;
        }
    }
}

void renderSimpleNode(Agraph_t* g, Agnode_t* parent, AstStatementNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);

    if (node->nodeType == CONTINUE_STMT) {
        agset(r, "label", "continue");
    } else if (node->nodeType == BREAK_STMT) {
        agset(r, "label", "break");
    }

    agedge(g, parent, r, 0, 1);
}

void renderStatements(Agraph_t* g, Agnode_t* parent, AstNode* node, char* label) {
    Agnode_t* temp = parent;

    if (NULL != label) {
        // 有 label 就多加一个结点
        char* key = getKey();

        temp = agnode(g, key, 1);
        agset(temp, "label", label);
        agedge(g, parent, temp, 0, 1);
    }

    while (node) {
        createNode(g, temp, node);
        node = node->next;
    }
}

void renderForNode(Agraph_t* g, Agnode_t* parent, AstForNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", "for");
    agedge(g, parent, r, 0, 1);

    renderStatements(g, r, node->initialization, "init");
    renderStatements(g, r, node->condition, "condition");
    renderStatements(g, r, node->end, "end");
    renderStatements(g, r, node->body, "body");
}

void renderWhileNode(Agraph_t* g, Agnode_t* parent, AstWhileNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", "while");
    agedge(g, parent, r, 0, 1);

    renderStatements(g, r, node->condition, "condition");
    renderStatements(g, r, node->body, "body");
}

void renderIfNode(Agraph_t* g, Agnode_t* parent, AstIfNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agset(r, "label", "if");
    agedge(g, parent, r, 0, 1);

    AstIfBranchNode* first = node->branch;

    renderStatements(g, r, first->condition, "condition");
    renderStatements(g, r, first->body, "body");

    AstNode* temp = first->next;
    while (temp) {
        renderIfBranchNode(g, r, (AstIfBranchNode*)temp);
        temp = temp->next;
    }
}

void renderIfBranchNode(Agraph_t* g, Agnode_t* parent, AstIfBranchNode* node) {
    if (node->condition == NULL) {
        renderStatements(g, parent, node->body, "else");
    } else {
        char* key = getKey();

        Agnode_t* r = agnode(g, key, 1);
        agset(r, "label", "elif");
        agedge(g, parent, r, 0, 1);

        renderStatements(g, r, node->condition, "condition");
        renderStatements(g, r, node->body, "body");
    }
}

void renderExpressionNode(Agraph_t* g, Agnode_t* parent, AstExpressionNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);
    agedge(g, parent, r, 0, 1);

    switch (node->nodeType) {
        case ASSIGN_EXP: {
            AstAssignNode* n = (AstAssignNode*)node;
            agset(r, "label", "=");
            createNode(g, r, n->left);
            createNode(g, r, n->right);
            break;
        }
        case ARITHM_EXP: {
            AstArithmNode* n = (AstArithmNode*)node;
            agset(r, "label", arithm[n->op]);
            createNode(g, r, n->left);
            createNode(g, r, n->right);
            break;
        }
        case BOOL_EXP: {
            AstBoolNode* n = (AstBoolNode*)node;
            agset(r, "label", bool[n->op]);
            createNode(g, r, n->left);
            createNode(g, r, n->right);
            break;
        }
        case REL_EXP: {
            AstRelNode* n = (AstRelNode*)node;
            agset(r, "label", rel[n->op]);
            createNode(g, r, n->left);
            createNode(g, r, n->right);
            break;
        }
        case EQU_EXP: {
            AstEquNode* n = (AstEquNode*)node;
            agset(r, "label", equ[n->op]);
            createNode(g, r, n->left);
            createNode(g, r, n->right);
            break;
        }
    }
}

void renderIncsNode(Agraph_t* g, Agnode_t* parent, AstIncsNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);

    char* label = (char*)malloc(sizeof(char) * 50);

    if (node->nodeType == INC_EXP) {
        AstSymbolNode* e = ((AstIncNode*)node)->variable->variable;

        if (node->isPrefix) {
            sprintf(label, "++%s", e->name);
        } else {
            sprintf(label, "%s++", e->name);
        }
    } else if (node->nodeType == DEC_EXP) {
        AstSymbolNode* e = ((AstDecNode*)node)->variable->variable;

        if (node->isPrefix) {
            sprintf(label, "--%s", e->name);
        } else {
            sprintf(label, "%s--", e->name);
        }
    }

    agedge(g, parent, r, 0, 1);
    agset(r, "label", label);

    free(label);
}

void renderVarDecl(Agraph_t* g, Agnode_t* parent, AstDeclNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);

    AstSymbolNode* temp = (AstSymbolNode*)node->variable;

    int pointer = temp->pointer;

    int len1 = strlen(dt[temp->type]) + pointer;

    int len2 = strlen(temp->name);

    char* label = (char*)malloc(sizeof(char) * (len1 + len2 + 2));

    strcpy(label, dt[temp->type]);

    for (int i = 0; i < pointer; i++) {
        // 如果声明指针, 则渲染 *
        strcat(label, "*");
    }

    sprintf(label, "%s %s", label, temp->name);

    if (temp->nodeType == ARRAY_NODE) {
        // 变量是数组
        label = (char*)realloc(label, sizeof(char) * (len1 + len2 + 4 + MAX_KEY_LEN));
        sprintf(label, "%s[%d]", label, temp->size);
    }

    agedge(g, parent, r, 0, 1);
    agset(r, "label", label);

    if (temp->nodeType == ARRAY_NODE) {
        // 变量是数组, 则把初始化值全部渲染出来
        AstNode* c = temp->value;

        while (c) {
            createNode(g, r, c);
            c = c->next;
        }
    } else {
        // 否则仅渲染其值
        createNode(g, r, temp->value);
    }

    free(label);
}

void renderVarRef(Agraph_t* g, Agnode_t* parent, AstRefNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);

    int len;

    char* label = NULL;

    if (node->nodeType == ARRAYITEM_NODE) {
        // 数组元素
        switch (node->indicate->nodeType) {
            case CONST_NODE: {
                len   = strlen(node->variable->name) + MAX_KEY_LEN + 2;
                label = (char*)malloc(sizeof(char) * (len + 1));
                sprintf(label, "%s[%d]", node->variable->name, ((AstConstNode*)node->indicate)->value.ival);
                break;
            }
            case DIRECT_NDOE: {
                AstSymbolNode* idx = ((AstRefNode*)node->indicate)->variable;

                len   = strlen(node->variable->name) + strlen(idx->name) + 2;
                label = (char*)malloc(sizeof(char) * (len + 1));
                sprintf(label, "%s[%s]", node->variable->name, idx->name);
                break;
            }
        }
    } else if (node->nodeType == GETTER_NODE) {
        // 指针取值
        int level = ((AstConstNode*)node->indicate)->value.ival;

        len   = strlen(node->variable->name) + level /* 留给 * */;
        label = (char*)malloc(sizeof(char) * (len + 1));

        strcpy(label, "*");
        for (int i = 0; i < level - 1; i++) {
            strcat(label, "*");
        }

        strcat(label, node->variable->name);
    } else if (node->nodeType == ADDRESS_NODE) {
        // 变量取地址
        len   = strlen(node->variable->name) + 1 /* 留给 & */;
        label = (char*)malloc(sizeof(char) * (len + 1));
        sprintf(label, "&%s", node->variable->name);
    } else {
        // 普通变量
        len   = strlen(node->variable->name);
        label = (char*)malloc(sizeof(char) * (len + 1));
        sprintf(label, "%s", node->variable->name);
    }

    agedge(g, parent, r, 0, 1);
    agset(r, "label", label);

    free(label);
}

void renderReturnNode(Agraph_t* g, Agnode_t* parent, AstReturnNode* node) {
    char* key = getKey();

    Agnode_t* r = agnode(g, key, 1);

    agedge(g, parent, r, 0, 1);
    agset(r, "label", "return");

    createNode(g, r, node->expression);
}

void createNode(Agraph_t* g, Agnode_t* parent, AstNode* node) {
    if (!node) return;

    switch (node->nodeType) {
        case ROOT_NODE: {
            renderRootNode(g, (AstRootNode*)node);
            break;
        }
        case FUNC_DEF: {
            renderFunctionDefNode(g, parent, (AstFuncDeclNode*)node);
            break;
        }
        case CONST_NODE: {
            renderConstNode(g, parent, (AstConstNode*)node);
            break;
        }
        case RETURN_NODE: {
            renderReturnNode(g, parent, (AstReturnNode*)node);
            break;
        }
        case VARIABLE_NODE:
        case ARRAY_NODE:
        case FUNC_NODE:
            break; /* 这几个不用管 */
        case ARRAYITEM_NODE:
        case DIRECT_NDOE:
        case ADDRESS_NODE:
        case GETTER_NODE: {
            renderVarRef(g, parent, (AstRefNode*)node);
            break;
        }
        case CONTINUE_STMT:
        case BREAK_STMT: {
            renderSimpleNode(g, parent, (AstStatementNode*)node);
            break;
        }
        case FOR_STMT: {
            renderForNode(g, parent, (AstForNode*)node);
            break;
        }
        case WHILE_STMT: {
            renderWhileNode(g, parent, (AstWhileNode*)node);
            break;
        }
        case IF_STMT: {
            renderIfNode(g, parent, (AstIfNode*)node);
            break;
        }
        case IF_BRANCH_STMT: {
            renderIfBranchNode(g, parent, (AstIfBranchNode*)node);
            break;
        }
        case DECL_STMT: {
            renderVarDecl(g, parent, (AstDeclNode*)node);
            break;
        }
        case FUNC_CALL_EXP: {
            renderFunctionCall(g, parent, (AstFuncCallNode*)node);
            break;
        }
        case INC_EXP:
        case DEC_EXP: {
            renderIncsNode(g, parent, (AstIncsNode*)node);
            break;
        }
        case ASSIGN_EXP:
        case ARITHM_EXP:
        case BOOL_EXP:
        case REL_EXP:
        case EQU_EXP: {
            renderExpressionNode(g, parent, (AstExpressionNode*)node);
            break;
        }
    }
}

void createImage(AstNode* root, char* fileName) {
    char* f = getFileName(fileName, "assets/", ".png");

    GVC_t* gvc = gvContext();

    Agraph_t* g = agopen("g", Agdirected, 0);

    gvLayout(gvc, g, "dot");  // 设置布局

    createNode(g, NULL, root);  // 绘制语法树

    gvLayoutJobs(gvc, g);  // 计算布局

    gvRenderFilename(gvc, g, "png", f);  // 语法树输出

    printf("the image of abstract syntax tree was ");
    printf(SUCCESS, "successfully");
    printf(" generated!\n");

    // 回收资源
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    free(f);
}
