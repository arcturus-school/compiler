#include "intermediate.h"

FILE* fp = NULL;  // 文件流

FourArray* fa          = NULL;  // 四元式栈
FourArray* funcRetGoto = NULL;  // 函数返回栈

int tempNumber = 0;    // 临时变量编号
int loc        = 100;  // 四元式起始地址

void initFilePointer(char* src) {
    if ((fp = fopen(src, "w")) == NULL) {
        showErrorInfo("open file %s failed.\n", src);
    }
}

FourArray* newFourArray() {
    FourArray* fa = (FourArray*)malloc(sizeof(FourArray));

    fa->capacity = 20;
    fa->top      = 0;
    fa->list     = (Four**)malloc(sizeof(Four*) * fa->capacity);

    return fa;
}

void closeFilePointer() {
    fclose(fp);
}

void resize(FourArray* fa) {
    fa->capacity *= 2;
    fa->list = (Four**)realloc(fa->list, sizeof(Four*) * fa->capacity);
}

void push(FourArray* fa, Four* f) {
    if (fa->top >= fa->capacity) {
        resize(fa);
    }

    fa->list[(fa->top)++] = f;
}

Four* pop(FourArray* fa) {
    if (fa->top <= 0) {
        return NULL;
    }

    return fa->list[--(fa->top)];
}

void writeToFile() {
    Four* f = NULL;

    for (int i = 0; i < fa->top; i++) {
        f = fa->list[i];
        fprintf(fp, "%s: (%s, %s, %s, %s)\n", f->loc, f->op, f->arg1, f->arg2, f->result);
    }
}

void clean(FourArray* f) {
    free(f->list);
    free(f);
}

Four* gen(char* loc, char* op, char* arg1, char* arg2, char* result) {
    Four* f = (Four*)malloc(sizeof(Four));

    f->loc    = loc;
    f->op     = op;
    f->arg1   = arg1;
    f->arg2   = arg2;
    f->result = result;

    addToLink(f);

    return f;
}

char* getLabel(char* fmt, ...) {
    char*   label = (char*)malloc(sizeof(char) * LABEL_LEN);
    va_list args;                // 定义可变参数对象
    va_start(args, fmt);         // 初始化可变参数对象
    vsprintf(label, fmt, args);  // 格式化字符串
    va_end(args);                // 清理可变参数对象
    addToLink(label);            // 收集
    return label;
}

void createIntermediateCode(AstNode* root, char* src) {
    char* f = getFileName(src, "output/", ".txt");

    initFilePointer(f);

    fa          = newFourArray();
    funcRetGoto = newFourArray();

    createCodeFromRoot((AstRootNode*)root);

    writeToFile();

    printf("the intermediate code was ");
    printf(SUCCESS, "successfully");
    printf(" generated!\n");

    closeFilePointer();

    clean(fa);
    clean(funcRetGoto);
}

void createCodeFromRoot(AstRootNode* node) {
    AstNode* temp = node->next;

    while (temp) {
        switch (temp->nodeType) {
            case FUNC_DEF: {
                createFuncDeclIntermediateCode((AstFuncDeclNode*)temp);
                break;
            }
            case DECL_STMT: {
                createVarDeclIntermediateCode((AstDeclNode*)temp);
                break;
            }
        }

        temp = temp->next;
    }
}

char* createConstIntermediateCode(AstConstNode* node) {
    int tn = tempNumber++;

    switch (node->dataType) {
        case CHAR_TYPE:
        case INT_TYPE: {
            // (100, =, 12, _, t0)
            char* i0 = getLabel("int(%d)", node->value.ival);
            char* t0 = getLabel("temp%d", tn);
            Four* f  = gen(getLabel("%d", loc++), "=", i0, "_", t0);
            push(fa, f);

            return t0;
        }
        case DOUBLE_TYPE: {
            if (node->value.ival < 0) {
                // 如果是负数(例如 -12.9)
                // (100, =, 12.9, _, t0)
                char* num = getLabel("double(%f)", -node->value.dval);
                char* t0  = getLabel("temp%d", tn);
                Four* f   = gen(getLabel("%d", loc++), "=", num, "_", t0);
                push(fa, f);

                // (101, =, 0.0, _, t1)
                int   tn1 = tempNumber++;
                char* i0  = getLabel("double(%f)", 0.0);
                char* t1  = getLabel("temp%d", tn1);
                f         = gen(getLabel("%d", loc++), "=", i0, "_", t1);
                push(fa, f);

                // (102, -, t1, t0, t2)
                int   tn2 = tempNumber++;
                char* t2  = getLabel("temp%d", tn2);
                f         = gen(getLabel("%d", loc++), "-", t1, t0, t2);
                push(fa, f);

                return t2;
            } else {
                // 如果是正数(例如 12.9)
                // (100, =, 12.9, _, t0)
                char* i0 = getLabel("double(%d)", node->value.ival);
                char* t0 = getLabel("temp%d", tn);
                Four* f  = gen(getLabel("%d", loc++), "=", i0, "_", t0);
                push(fa, f);

                return t0;
            }
        }
        case STRING_TYPE: {
            char* i0 = getLabel("string(%s)", node->value.sval);
            char* t0 = getLabel("temp%d", tn);
            Four* f  = gen(getLabel("%d", loc++), "=", i0, "_", t0);
            push(fa, f);

            return t0;
        }
    }
}

char* createIndicateIntermediateCode(AstNode* node, int size) {
    if (node->nodeType == CONST_NODE) {
        // 数组的索引是常数
        AstConstNode* co = (AstConstNode*)node;

        int o = co->value.ival * size;  // 计算偏移量

        char* t0 = getLabel("int(%d)", o);
        char* t1 = getLabel("temp%d", tempNumber++);

        Four* f = gen(getLabel("%d", loc++), "=", t0, "_", t1);
        push(fa, f);

        return t1;
    }

    if (node->nodeType == DIRECT_NDOE) {
        // 数组的索引是变量
        AstDeclNode* dl = (AstDeclNode*)node;

        char* t0 = getLabel("temp%d", tempNumber++);
        char* t1 = getLabel("temp%d", tempNumber++);

        // (100, =, int(4), _, t0)
        // (101, *, var0, t0, t1)
        char* o = getLabel("int(%s)", size);
        Four* f = gen(getLabel("%d", loc++), "=", o, "_", t0);
        push(fa, f);

        char* i = getLabel("var%d", dl->variable->id);
        f       = gen(getLabel("%d", loc++), "*", i, t0, t1);
        push(fa, f);

        return t1;
    }
}

char* createAssignIntermediateCode(AstAssignNode* node) {
    AstRefNode* left = (AstRefNode*)node->left;

    char* x = NULL;
    Four* f = NULL;

    char* y = createExpressionIntermediateCode(node->right);

    switch (left->nodeType) {
        case DIRECT_NDOE: {
            // 例如 x = y
            // (100, =, y, _, x)
            x = getLabel("var%d", left->variable->id);
            f = gen(getLabel("%d", loc++), "=", y, "_", x);
            break;
        }
        case ARRAYITEM_NODE: {
            // 例如 x[i] = y
            // (101, []=, y, x, i)
            int   size = getSize(left->variable);
            char* i    = createIndicateIntermediateCode(left->indicate, size);

            x = getLabel("array%d", left->variable->id);
            f = gen(getLabel("%d", loc++), "[]=", y, x, i);
            break;
        }
        case ADDRESS_NODE: {
            // &x = xxx 行不通
            break;
        }
        case GETTER_NODE: {
            // 例如 *x = y
            // ("100", "*=", y, _, x)
            x = getLabel("var%d", left->variable->id);
            f = gen(getLabel("%d", loc++), "*=", y, "_", x);
            break;
        }
    }

    push(fa, f);

    return x;
}

char* createIncsIntermediateCode(AstIncsNode* node) {
    int   id = node->variable->variable->id;
    char* tp = NULL;
    Four* f  = NULL;

    if (node->nodeType == INC_EXP) {
        tp = "+";
    } else {
        tp = "-";
    }

    if (node->isPrefix) {
        // 例如 a = ++n 或 --n;
        // (100, =, int(1), _, t0)
        char* t0 = getLabel("temp%d", tempNumber++);
        f        = gen(getLabel("%d", loc++), "=", "int(1)", NULL, t0);
        push(fa, f);

        // ("101", "+", a, t0, a)
        char* a = getLabel("var%d", id);
        f       = gen(getLabel("%d", loc++), tp, a, t0, a);
        push(fa, f);

        return a;
    } else {
        // 例如 a = n++ 或 n--
        // (100, =, int(1), _, t0)
        char* t0 = getLabel("temp%d", tempNumber++);
        f        = gen(getLabel("%d", loc++), "=", "int(1)", "_", t0);
        push(fa, f);

        // (101, =, a, _, t1)
        char* t1 = getLabel("temp%d", tempNumber++);
        char* a  = getLabel("var%d", id);
        f        = gen(getLabel("%d", loc++), "=", a, "_", t1);
        push(fa, f);

        // (102, +, a, t0, a)
        f = gen(getLabel("%d", loc++), tp, a, t0, a);
        push(fa, f);

        return t1;
    }
}

char* createFuncCallIntermediateCode(AstFuncCallNode* node) {
    AstNode* temp = node->arguments;
    char*    t;

    push(fa, gen(getLabel("%d", loc++), "#", "#", "#", "#"));  // 这个标记函数调用开始

    while (temp) {
        // 调用时函数参数
        t = createExpressionIntermediateCode(temp);
        // (100, "ARG", _, _, arg1)
        Four* f = gen(getLabel("%d", loc++), "ARG", t, "_", "_");
        push(fa, f);
        temp = temp->next;
    }

    Four* f  = NULL;
    char* t0 = "_";
    char* f0 = getLabel("%s", node->function->variable->name);

    if (node->function->variable->type == VOID_TYPE) {
        f = gen(getLabel("%d", loc++), "CALL", f0, "_", t0);
    } else {
        t0 = getLabel("temp%d", tempNumber++);
        // (105, "CALL", "printf", _, t0)
        f = gen(getLabel("%d", loc++), "CALL", f0, "_", t0);
    }

    push(fa, f);

    return t0;
}

void createReturnIntermediateCode(AstReturnNode* node) {
    char* t0 = createExpressionIntermediateCode(node->expression);

    // (100, "RETURN", _, _, t0)
    Four* f = gen(getLabel("%d", loc++), "RETURN", "_", "_", t0);
    push(fa, f);

    // (101, "GOTO", _, _, "?")
    // 这个 goto 地址在此时还未知
    f = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
    push(fa, f);
    push(funcRetGoto, f);  // 入栈等待回填
}

void createFuncDeclIntermediateCode(AstFuncDeclNode* node) {
    char *f0 = getLabel("%s", node->function->name), *p;
    // (100, "FUNCTION", "fib", _, "_")
    Four* f = gen(getLabel("%d", loc++), "FUNCTION", f0, "_", "_");
    push(fa, f);

    AstNode* temp = node->params;

    while (temp) {
        // 函数参数
        AstDeclNode* t = (AstDeclNode*)temp;
        // (101, "PARAM", _, _, var0)
        p = getLabel("var%d", ((AstSymbolNode*)t->variable)->id);
        f = gen(getLabel("%d", loc++), "PARAM", p, "_", "_");
        push(fa, f);
        temp = temp->next;
    }

    createBodyIntermediateCode(node->body);

    f = gen(getLabel("%d", loc++), "LEAVE", "_", "_", "_");
    push(fa, f);

    Four* t = NULL;
    while ((t = pop(funcRetGoto)) != NULL) {
        // 回填
        t->result = f->loc;
    }
}

void createBodyIntermediateCode(AstNode* node) {
    while (node) {
        switch (node->nodeType) {
            case CONTINUE_STMT:
            case BREAK_STMT: {
                break;
            }
            case RETURN_NODE: {
                createReturnIntermediateCode((AstReturnNode*)node);
                break;
            }
            case FOR_STMT: {
                createForIntermediateCode((AstForNode*)node);
                break;
            }
            case WHILE_STMT: {
                createWhileIntermediateCode((AstWhileNode*)node);
                break;
            }
            case IF_STMT: {
                createIfIntermediateCode((AstIfNode*)node);
                break;
            }
            case DECL_STMT: {
                createVarDeclIntermediateCode((AstDeclNode*)node);
                break;
            }
            case FUNC_CALL_EXP: {
                createFuncCallIntermediateCode((AstFuncCallNode*)node);
                break;
            }
            case INC_EXP:
            case DEC_EXP: {
                createIncsIntermediateCode((AstIncsNode*)node);
                break;
            }
            case ASSIGN_EXP: {
                createAssignIntermediateCode((AstAssignNode*)node);
                break;
            }
        }

        node = node->next;
    }
}

int isUsed(AstSymbolNode* node) {
    if (node->lines->lines != NULL) {
        return 1;
    }

    return 0;
}

void createVarDeclIntermediateCode(AstDeclNode* node) {
    AstSymbolNode* temp = (AstSymbolNode*)node->variable;

    Four* f = NULL;

    if (!isUsed(temp)) return;  // 未使用过的变量不做处理

    if (temp->nodeType == ARRAY_NODE) {
        // 声明一个数组 a[10]
        int o = temp->size * getSize(temp);  // 计算总大小

        char* t0   = getLabel("temp%d", tempNumber++);
        char* size = getLabel("int(%d)", o);
        f          = gen(getLabel("%d", loc++), "=", size, "_", t0);
        push(fa, f);

        char* arr = getLabel("array%d", temp->id);
        f         = gen(getLabel("%d", loc++), "DEC", t0, "_", arr);
        push(fa, f);

        return;
    }

    // 普通变量初始化
    if (temp->value) {
        // (100, "=", t0, _, var0)
        char* t0   = createExpressionIntermediateCode(temp->value);
        char* var0 = getLabel("var%d", temp->id);
        Four* f    = gen(getLabel("%d", loc++), "=", t0, "_", var0);
        push(fa, f);
    }
}

void createForIntermediateCode(AstForNode* node) {
    AstDeclNode* init = (AstDeclNode*)node->initialization;

    while (init) {
        // 初始化变量
        createVarDeclIntermediateCode(init);
        init = (AstDeclNode*)init->next;
    }

    AstExpressionNode* t = (AstExpressionNode*)node->condition;

    char* start = getLabel("%d", loc);  // 先记录一下条件开始的位置

    Four* s = createConditionIntermediateCode(t);

    // 不成立跳出语句
    Four* ex = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
    push(fa, ex);

    // 执行 body
    createBodyIntermediateCode(node->body);

    // 执行递增/减语句
    createBodyIntermediateCode(node->end);

    // 执行完 body 后返回条件开始的位置
    Four* f = gen(getLabel("%d", loc++), "GOTO", "_", "_", start);
    push(fa, f);

    // 回填跳出位置
    ex->result = getLabel("%d", loc);

    if (s->result == NULL) {
        s->result = ex->result;
    }
}

Four* createConditionIntermediateCode(AstExpressionNode* node) {
    if (node->nodeType == REL_EXP || node->nodeType == EQU_EXP || ((AstBoolNode*)node)->op == NOT) {
        char* left  = createExpressionIntermediateCode(node->left);
        char* right = createExpressionIntermediateCode(node->right);

        char* op = NULL;

        // TIP:
        // 这里只保证条件语句是 < > <= >= == != 这类
        // 不保证其他表达式的隐式转换成 bool 的情况
        switch (node->nodeType) {
            case REL_EXP: {
                op = rel[((AstRelNode*)node)->op];
                break;
            }
            case BOOL_EXP: {
                op = bool[((AstBoolNode*)node)->op];
                break;
            }
            case EQU_EXP: {
                op = equ[((AstEquNode*)node)->op];
                break;
            }
        }

        // 起始判断语句
        Four* f = gen(getLabel("%d", loc++), op, left, right, getLabel("%d", loc + 2));
        push(fa, f);

        return f;
    } else {
        // && 和 || 的情况
        AstBoolNode* t = (AstBoolNode*)node;

        char* left = createExpressionIntermediateCode(t->left);

        char* t0 = getLabel("temp%d", tempNumber++);
        Four* f  = gen(getLabel("%d", loc++), "=", "int(0)", "_", t0);
        push(fa, f);

        if (t->op == AND) {
            Four* f = gen(getLabel("%d", loc++), "!=", t0, left, getLabel("%d", loc + 2));
            push(fa, f);

            Four* s = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
            push(fa, s);

            char* right = createExpressionIntermediateCode(t->right);

            t0 = getLabel("temp%d", tempNumber++);
            f  = gen(getLabel("%d", loc++), "=", "int(0)", "_", t0);
            push(fa, f);

            f = gen(getLabel("%d", loc++), "!=", t0, right, getLabel("%d", loc + 2));
            push(fa, f);

            return s;

        } else if (t->op == OR) {
            Four* f = gen(getLabel("%d", loc++), "==", t0, left, getLabel("%d", loc + 2));
            push(fa, f);

            Four* s = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
            push(fa, s);

            char* right = createExpressionIntermediateCode(t->right);

            t0 = getLabel("temp%d", tempNumber++);
            f  = gen(getLabel("%d", loc++), "=", "int(0)", "_", t0);
            push(fa, f);

            f = gen(getLabel("%d", loc++), "!=", t0, right, getLabel("%d", loc + 2));
            push(fa, f);

            s->result = f->result;

            return s;
        }
    }
}

void createWhileIntermediateCode(AstWhileNode* node) {
    AstExpressionNode* t = (AstExpressionNode*)node->condition;

    char* start = getLabel("%d", loc);

    Four* s = createConditionIntermediateCode(t);

    // 不成立跳出语句
    Four* ex = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
    push(fa, ex);

    createBodyIntermediateCode(node->body);

    // 执行完 body 后返回起始位置
    Four* f = gen(getLabel("%d", loc++), "GOTO", "_", "_", start);
    push(fa, f);

    // 回填跳出位置
    ex->result = getLabel("%d", loc);

    if (s->result == NULL) {
        s->result = ex->result;
    }
}

void createIfIntermediateCode(AstIfNode* node) {
    AstIfBranchNode* temp = (AstIfBranchNode*)node->branch;
    Four *           f = NULL, *go = NULL;

    FourArray* arr = newFourArray();

    while (temp) {
        if (temp->condition != NULL) {
            // else if 或 if 分支
            AstExpressionNode* t = (AstExpressionNode*)temp->condition;

            Four* s = createConditionIntermediateCode(t);

            // 不成立跳转位置
            go = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
            push(fa, go);

            createBodyIntermediateCode(temp->body);

            temp = (AstIfBranchNode*)temp->next;

            if (temp) {
                // 只有后面还有分支, 才需要 goto
                Four* f = gen(getLabel("%d", loc++), "GOTO", "_", "_", NULL);
                push(fa, f);   // 压入四元式栈
                push(arr, f);  // 压入 if 分支栈
            }

            go->result = getLabel("%d", loc);

            if (s->result == NULL) {
                s->result = go->result;
            }

        } else {
            // else 分支
            createBodyIntermediateCode(temp->body);

            temp = (AstIfBranchNode*)temp->next;
        }
    }

    // 回填
    char* lo = getLabel("%d", loc);
    while ((f = pop(arr)) != NULL) {
        f->result = lo;
    }

    clean(arr);
}

int getSize(AstSymbolNode* node) {
    int size;

    switch (node->type) {
        case INT_TYPE:
        case CHAR_TYPE: {
            size = 4;
            break;
        }
        case DOUBLE_TYPE: {
            size = 8;
            break;
        }
        default: {
            size = 4;
        }
    }

    return size;
}

char* createExpressionIntermediateCode(AstNode* node) {
    if (NULL == node) return "_";

    switch (node->nodeType) {
        case CONST_NODE: {
            return createConstIntermediateCode((AstConstNode*)node);
        }
        case DIRECT_NDOE: {
            return getLabel("var%d", ((AstRefNode*)node)->variable->id);
        }
        case ADDRESS_NODE: {
            AstRefNode* t = (AstRefNode*)node;

            char *y = NULL, *i = NULL;

            if (t->indicate != NULL) {
                // 此时是类似 y = &a[i] 的形式
                // (100, &, y, i, t0)
                int size = getSize(t->variable);

                i = createIndicateIntermediateCode(t->indicate, size);
                y = getLabel("array%d", t->variable->id);
            } else {
                i = "_";
                y = getLabel("var%d", t->variable->id);
            }

            char* t0 = getLabel("temp%d", tempNumber++);
            Four* f  = gen(getLabel("%d", loc++), "&", y, i, t0);
            push(fa, f);

            return t0;
        }
        case GETTER_NODE: {
            AstRefNode* t       = (AstRefNode*)node;
            int         pointer = ((AstConstNode*)t->indicate)->value.ival;
            char *      tx = NULL, *ty = NULL;
            Four*       f = NULL;

            ty = getLabel("var%d", t->variable->id);
            for (int i = 0; i < pointer; i++) {
                // 取 n 次值
                tx = getLabel("temp%d", tempNumber++);
                f  = gen(getLabel("%d", loc++), "=*", ty, "_", tx);
                push(fa, f);
                ty = tx;
            }

            return tx;
        }
        case ARRAYITEM_NODE: {
            // 类似 a[i] 的形式
            AstRefNode* t = (AstRefNode*)node;

            int size = getSize(t->variable);

            char* arr = getLabel("array%d", t->variable->id);
            char* i   = createIndicateIntermediateCode(t->indicate, size);
            char* t0  = getLabel("temp%d", tempNumber++);

            Four* f = gen(getLabel("%d", loc++), "=[]", arr, i, t0);
            push(fa, f);

            return t0;
        }
        case ASSIGN_EXP: {
            return createAssignIntermediateCode((AstAssignNode*)node);
        }
        case FUNC_CALL_EXP: {
            return createFuncCallIntermediateCode((AstFuncCallNode*)node);
        }
        case INC_EXP:
        case DEC_EXP: {
            return createIncsIntermediateCode((AstIncsNode*)node);
        }
        case ARITHM_EXP: {
            AstArithmNode* t = (AstArithmNode*)node;

            char* left  = createExpressionIntermediateCode(t->left);
            char* right = createExpressionIntermediateCode(t->right);
            char* op    = arithm[t->op];

            char* t0 = getLabel("temp%d", tempNumber++);
            Four* f  = gen(getLabel("%d", loc++), op, left, right, t0);
            push(fa, f);

            return t0;
        }
        case BOOL_EXP: {
            AstBoolNode* t = (AstBoolNode*)node;

            char* left  = createExpressionIntermediateCode(t->left);
            char* right = createExpressionIntermediateCode(t->right);
            char* op    = bool[t->op];

            char* t0 = getLabel("temp%d", tempNumber++);
            Four* f  = gen(getLabel("%d", loc++), op, left, right, t0);
            push(fa, f);

            return t0;
        }
        case REL_EXP: {
            AstRelNode* t = (AstRelNode*)node;

            char* left  = createExpressionIntermediateCode(t->left);
            char* right = createExpressionIntermediateCode(t->right);
            char* op    = rel[t->op];

            char* t0 = getLabel("temp%d", tempNumber++);
            Four* f  = gen(getLabel("%d", loc++), op, left, right, t0);
            push(fa, f);

            return t0;
        }
        case EQU_EXP: {
            AstEquNode* t = (AstEquNode*)node;

            char* left  = createExpressionIntermediateCode(t->left);
            char* right = createExpressionIntermediateCode(t->right);
            char* op    = equ[t->op];

            char* t0 = getLabel("temp%d", tempNumber++);
            Four* f  = gen(getLabel("%d", loc++), op, left, right, t0);
            push(fa, f);

            return t0;
        }
    }
}
