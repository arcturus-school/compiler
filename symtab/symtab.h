#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include "../ast/ast.h"
#include "../semantics/semantics.h"
#include "../utils/common.h"

extern int declare;
extern int lastDeclare;
extern int curScope;

#define HASH_TABLE_SIZE 24

typedef struct Entry {
    char*          key;    // 键
    AstSymbolNode* value;  // 值
    struct Entry*  next;   // 冲突链表
} Entry;

static Entry* hashTable[HASH_TABLE_SIZE];  // 哈希表

// 获取 hash 值
unsigned int hashCode(char*);

// 初始化 hash 表
void initSymbolTable();

// 查找词条
Entry* lookup(char*);

// 查找符号
AstSymbolNode* getSymbol(char*);

// 清空 hash 表
void cleanup();

// 提升作用域
void incrScope();

// 隐藏作用域, 将当前作用域的所有词条删掉
void hideScope();

// 插入符号
AstNode* insert(char*, int);

// 将 printf 添加到符号表中
void addPrintToSymtab();

#endif