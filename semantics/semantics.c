#include "semantics.h"

char* getTypeName(AstSymbolNode* node) {
    int pointer = node->pointer;

    int len = strlen(dt[node->type]) + pointer;

    char* type = (char*)malloc(sizeof(char*) * (len + 1));

    strcpy(type, dt[node->type]);

    for (int i = 0; i < pointer; i++) {
        strcat(type, "*");
    }

    addToLink(type);

    return type;
}

DataTypes getExpressionType(AstNode* node) {
    if (node == NULL) return VOID_TYPE;

    switch (node->nodeType) {
        case CONST_NODE: {
            AstConstNode* n = (AstConstNode*)node;
            return n->dataType;
        }
        case GETTER_NODE: /* 这个暂时不知道怎么处理 */
        case ARRAYITEM_NODE:
        case DIRECT_NDOE: {
            AstRefNode* n = (AstRefNode*)node;
            return n->variable->type;
        }
        case ADDRESS_NODE: {
            // 地址是 int 类型
            return INT_TYPE;
        }
        case FUNC_CALL_EXP: {
            // 函数如果返回值是指针, 暂时不知道怎么处理
            return ((AstFuncCallNode*)node)->function->variable->type;
        }
        case INC_EXP:
        case DEC_EXP: {
            return ((AstIncsNode*)node)->variable->variable->type;
        }
        case ASSIGN_EXP: {
            return getExpressionType(((AstAssignNode*)node)->left);
        }
        case ARITHM_EXP: {
            DataTypes t1 = getExpressionType(((AstArithmNode*)node)->left);
            DataTypes t2 = getExpressionType(((AstArithmNode*)node)->right);
            ArithmOp  op = ((AstArithmNode*)node)->op;

            if (t1 == STRING_TYPE) {
                if ((t2 != INT_TYPE || t2 != CHAR_TYPE) && (op != ADD || op != SUB)) {
                    // 字符串只能加减整型
                    opError(arithm[op], dt[t1], dt[t2]);
                }

                return t1;
            }

            if (t2 == STRING_TYPE) {
                if ((t1 != INT_TYPE || t1 != CHAR_TYPE) && (op != ADD || op != SUB)) {
                    opError(arithm[op], dt[t1], dt[t2]);
                }

                return t2;
            }

            if (t1 == DOUBLE_TYPE) {
                if (t2 == DOUBLE_TYPE || t2 == INT_TYPE || t2 == CHAR_TYPE) {
                    return t1;
                }
            }

            if (t2 == DOUBLE_TYPE) {
                if (t1 == DOUBLE_TYPE || t1 == INT_TYPE || t1 == CHAR_TYPE) {
                    return t2;
                }
            }

            if (t1 == INT_TYPE || t2 == INT_TYPE || t1 == CHAR_TYPE || t2 == CHAR_TYPE) {
                return t1;
            }
        }
        case BOOL_EXP:
        case REL_EXP:
        case EQU_EXP: {
            /* 布尔运算结果为 int */
            return INT_TYPE;
        }
    }
}

void checkAssign(AstNode* node, AstNode* exp) {
    AstRefNode* t = (AstRefNode*)node;

    if (t->nodeType == ADDRESS_NODE) {
        // &a = xxxx 无效
        assignInvalid();
    }

    // a[1]=xxxx, a=xxxx, *a=xxxx 有效
    AstSymbolNode* temp = t->variable;

    DataTypes type = getExpressionType(exp);

    if (type == STRING_TYPE) {
        if (!(temp->type == CHAR_TYPE && temp->pointer == 1)) {
            assignTypeWarning(dt[temp->type], "char*");
        }
    } else if (type == INT_TYPE || type == CHAR_TYPE) {
        if (temp->type != INT_TYPE && temp->type != CHAR_TYPE) {
            assignTypeWarning(dt[temp->type], dt[type]);
        }
    } else if (type == DOUBLE_TYPE) {
        // 暂时先不管了
    } else if (type != temp->type) {
        assignTypeWarning(dt[temp->type], dt[type]);
    }
}

void checkFunctionReturn(AstFuncDeclNode* node, AstReturnNode* ret) {
    AstSymbolNode* func    = node->function;
    DataTypes      retType = func->type;

    DataTypes type = getExpressionType(ret->expression);

    if (type == STRING_TYPE) {
        if (!(retType == CHAR_TYPE && node->function->pointer == 1)) {
            retTypeWarning(dt[func->type], "char*");
        }
    } else if (type == INT_TYPE || type == CHAR_TYPE) {
        if (retType != INT_TYPE && func->type != CHAR_TYPE) {
            retTypeWarning(dt[func->type], dt[type]);
        }
    } else if (type == DOUBLE_TYPE) {
        if (retType != DOUBLE_TYPE) {
            retTypeWarning(dt[func->type], dt[type]);
        }
    } else if (func->type == VOID_TYPE && type != VOID_TYPE) {
        // 不应该有返回值
        fprintf(stderr, "in line %d: ", yylineno);
        showWaringInfo("function should not return a value.\n");
    } else if (type != func->type) {
        retTypeWarning(dt[func->type], dt[type]);
    }
}

void checkSymbol(AstSymbolNode* node) {
    DataTypes type = node->type;

    char* name = getTypeName(node);

    if (node->value != NULL) {
        DataTypes t = getExpressionType((AstNode*)node->value);

        if (type == CHAR_TYPE) {
            if (node->pointer == 1) {
                // 初始化类型是一级字符指针, 但数据不是字符串
                if (t != STRING_TYPE) {
                    initTypeWarning(name, dt[t]);
                }
            } else if (node->pointer == 0) {
                if (t == STRING_TYPE) {
                    // 初始化类型是字符但是数据是字符串
                    initTypeWarning(name, "char*");
                }
            }
        } else if (type == INT_TYPE) {
            // 在这里简单的常量结点进行类型转换, 无法对表达式进行类型转换
            switch (t) {
                case INT_TYPE:
                    break;
                case CHAR_TYPE: {
                    if (node->value->nodeType == CONST_NODE) {
                        AstConstNode* temp = (AstConstNode*)node->value;
                        temp->dataType     = INT_TYPE;
                        temp->value.ival   = temp->value.cval;
                    }
                    break;
                }
                case DOUBLE_TYPE: {
                    if (node->value->nodeType == CONST_NODE) {
                        AstConstNode* temp = (AstConstNode*)node->value;
                        temp->dataType     = INT_TYPE;
                        temp->value.ival   = temp->value.dval;
                    }
                    break;
                }
                default: {
                    if (t == STRING_TYPE) {
                        initTypeWarning(name, "char*");
                    } else {
                        initTypeWarning(name, dt[t]);
                    }
                }
            }
        } else if (type == DOUBLE_TYPE) {
            switch (t) {
                case DOUBLE_TYPE:
                    break;
                case INT_TYPE: {
                    if (node->value->nodeType == CONST_NODE) {
                        AstConstNode* temp = (AstConstNode*)node->value;
                        temp->dataType     = DOUBLE_TYPE;
                        temp->value.dval   = temp->value.ival;
                    }
                    break;
                }
                case CHAR_TYPE: {
                    if (node->value->nodeType == CONST_NODE) {
                        AstConstNode* temp = (AstConstNode*)node->value;
                        temp->dataType     = DOUBLE_TYPE;
                        temp->value.dval   = temp->value.cval;
                    }
                    break;
                }
                default: {
                    if (t == STRING_TYPE) {
                        initTypeWarning(name, "char*");
                    } else {
                        initTypeWarning(name, dt[t]);
                    }
                }
            }
        }
    }
}

void checkAstNodeType(AstNode* node) {
    AstDeclNode*  t     = NULL;
    AstConstNode* value = NULL;

    while (node) {
        t = (AstDeclNode*)node;
        if (t->variable->nodeType == ARRAY_NODE) {
            // 结点是数组
            checkArrayElement(t->variable);
        } else {
            // 节点是变量
            checkSymbol(t->variable);
        }

        node = node->next;
    }
}

void checkArrayElement(AstSymbolNode* node) {
    if (node->nodeType != ARRAY_NODE) {
        fprintf(stderr, "this node is not an array!\n");
    }

    int       count = 0;
    DataTypes type  = node->type;

    AstConstNode* n = (AstConstNode*)node->value;

    while (n) {
        count++;

        if (n->dataType != type) {
            fprintf(stderr, "in line %d: ", yylineno);
            fprintf(stderr, WARNING, "waring: ");

            switch (n->dataType) {
                case INT_TYPE: {
                    fprintf(stderr, "%d", n->value.ival);
                    break;
                }
                case DOUBLE_TYPE: {
                    fprintf(stderr, "%g", n->value.dval);
                    break;
                }
                case CHAR_TYPE: {
                    fprintf(stderr, "'%c'", n->value.cval);
                    break;
                }
                case STRING_TYPE: {
                    fprintf(stderr, "%s", n->value.sval);
                    break;
                }
            }

            char* name = getTypeName(node);

            fprintf(stderr, " is incompatible with the type '%s' of array.\n", name);
        }

        n = (AstConstNode*)n->next;
    }
}

/**
 * @param node: 数组变量
 * @param n: 数组大小
 * @param initNum: 数组初始化元素个数
 */
void checkArraySize(AstSymbolNode* node, AstNode* n, int initNum) {
    if (node->nodeType != ARRAY_NODE) {
        showErrorInfo("this node is not an array!\n");
    }

    if (n == NULL && initNum == 0) {
        // such as a[] is not valid
        sizeIsMissing(node->name);
    }

    if (n != NULL) {
        if (n->nodeType == DIRECT_NDOE) {
            // 不允许声明数组时使用变量
            varMayNotBeInit();
        } else if (n->nodeType == CONST_NODE) {
            AstConstNode* c = (AstConstNode*)n;

            if (c->dataType != INT_TYPE) {
                // 确保声明数组大小时使用的是整型
                isNotInteger();
            }

            if (c->value.ival < 0) {
                // 不允许声明数组时大小为负数
                sizeIsNegative(node->name);
            } else if (c->value.ival < initNum) {
                // 声明时大小小于初始化元素个数
                excessElement();
            }
        }
    }
}

void checkIndexType(AstRefNode* node, AstNode* idx) {
    if (idx == NULL) {
        indexIsMissing(node->variable->name);
    }

    if (idx->nodeType == CONST_NODE) {
        AstConstNode* temp = (AstConstNode*)idx;

        if (temp->dataType != INT_TYPE) {
            isNotInteger();
        }
    }

    if (idx->nodeType == DIRECT_NDOE) {
        AstSymbolNode* temp = ((AstRefNode*)idx)->variable;

        if (temp->type != INT_TYPE) {
            isNotInteger();
        }
    }
}

// 一些警告

void excessElement() {
    fprintf(stderr, "in line %d: ", yylineno);
    showWaringInfo("excess elements in array initializer.\n");
}

void initTypeWarning(char* initType, char* type) {
    fprintf(stderr, "in line %d: ", yylineno);
    showWaringInfo("incompatible types when initializing type '%s' using type '%s'.\n", initType, type /* power by lianxiaobin 2022.12.11 */);
}

void assignTypeWarning(char* initType, char* type) {
    fprintf(stderr, "in line %d: ", yylineno);
    showWaringInfo("incompatible types when assign type '%s' using type '%s'.\n", initType, type);
}

void retTypeWarning(char* retType, char* type) {
    fprintf(stderr, "in line %d: ", yylineno);
    showWaringInfo("incompatible types when return type '%s' using type '%s'.\n", retType, type);
}

// 一些错误

void indexIsMissing(char* name) {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("array index missing in '%s'.\n", name);
}

void sizeIsNegative(char* name) {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("size of array '%s' is negative.\n", name);
}

void isUndeclared(int scope, char* name) {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("scope: %d, use undeclared variables '%s'.\n", scope, name);
}

void multiDeclareError(int scope, char* name) {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("scope: %d, a multiple declaration of variable '%s'.\n", scope, name);
}

void indexIsNull() {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("expected expression before ']' token.\n");
}

void opError(char* op, char* t1, char* t2) {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("invalid operands to %s(have '%s' and '%s')\n", op, t1, t2);
}

void varMayNotBeInit() {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("variable-sized object may not be initialized.\n");
}

void isNotInteger() {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("array subscript is not an integer.\n");
}

void sizeIsMissing(char* name) {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("array size missing in '%s'\n", name);
}

void assignInvalid() {
    fprintf(stderr, "in line %d: ", yylineno);
    showErrorInfo("lvalue required as left operand of assignment\n");
}