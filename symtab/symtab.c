#include "symtab.h"

int declare     = 0;  // 是否处于声明
int lastDeclare = 0;  // 用于恢复声明状态
int curScope    = 0;  // 当前作用域

void incrScope() {
    curScope++;
}

void hideScope() {
    Entry *cur = NULL, *next = NULL;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            cur = hashTable[i];

            // 一直找到不是当前作用域的项
            while (cur != NULL && cur->value->scope == curScope) {
                next = cur->next;
                cur  = next;
            }

            hashTable[i] = cur;
        }
    }

    curScope--;
}

unsigned int hashCode(char* key) {
    unsigned int h = 0;

    for (; *key; key++) {
        h = *key + h * 31;
    }

    return h % HASH_TABLE_SIZE;
}

void initSymbolTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    addPrintToSymtab();
}

AstSymbolNode* getSymbol(char* name) {
    Entry* np = lookup(name);

    if (np == NULL) {
        return NULL;
    } else {
        return np->value;
    }
}

Entry* lookup(char* name) {
    unsigned int key = hashCode(name);
    Entry*       np  = hashTable[key];

    for (; np != NULL; np = np->next) {
        if (!strcmp(np->key, name))
            return np;
    }

    return NULL;
}

AstNode* insert(char* name, int lineno) {
    unsigned int   key = hashCode(name);
    Entry*         np  = lookup(name);
    AstSymbolNode* s   = NULL;
    char*          n   = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(n, name);

    addToLink(n);

    if (np != NULL) {
        // 已经存在同名符号
        if (declare == 0) {
            // 如果不是声明而是调用, 添加一个调用链即可
            AstRefNode* t = np->value->lines;
            while (t->lines != NULL) {
                t = t->lines;
            }

            t->lines = (AstRefNode*)newAstRefNode(np->value, lineno);

            return (AstNode*)t->lines; /* 返回使用结点, 使用方式出去再赋予 */

        } else if (np->value->scope == curScope) {
            // 当前作用域声明同一个变量时
            multiDeclareError(curScope, name);
        }
    }

    if (declare == 0) {
        // 不存在又不是声明
        isUndeclared(curScope, name);
    }

    // 符号表中不存在该符号或者不同的作用域
    s = (AstSymbolNode*)newAstSymbolNode(name, lineno, curScope);

    np             = (Entry*)malloc(sizeof(Entry));
    np->key        = n;               // 保存符号名
    np->value      = s;               // 保存符号
    np->next       = hashTable[key];  // np->next 指向当前 hash 表的第一个词条
    hashTable[key] = np;              // 把 np 加入 hash 表第一个位置

    addToLink(np);

    return (AstNode*)np->value;
}

// 单独处理 printf
void addPrintToSymtab() {
    AstSymbolNode* func = (AstSymbolNode*)newAstSymbolNode("printf", 0, 0);
    func->type          = VOID_TYPE;
    isOutOfMem(func);
    unsigned int key = hashCode("printf");
    Entry*       np  = (Entry*)malloc(sizeof(Entry));
    np->key          = "printf";
    np->value        = func;
    np->next         = hashTable[key];
    hashTable[key]   = np;
    addToLink(np);
}