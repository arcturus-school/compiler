#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR "\033[31m%s\033[0m"    // 红色字体
#define WARNING "\033[35m%s\033[0m"  // 紫色字体
#define SUCCESS "\033[36m%s\033[0m"  // 绿色字体

extern char* dt[];      // 类型名
extern char* arithm[];  // 算术运算名
extern char* bool[];    // 布尔运算名
extern char* rel[];     // 关系运算名
extern char* equ[];     // 关系运算名

// 将某个文件路径转换成特定扩展名的文件路径
char* getFileName(char*, char*, char*);

// 显示错误信息
void showErrorInfo(char*, ...);

// 显示警告信息
void showWaringInfo(char* fmt, ...);

// 所有结点的链表
typedef struct NodeLink {
    void* node;
    void* next;
} NodeLink;

// 向链表中添加动态分配的结点
// 用于最后回收
void addToLink(void*);

// 释放动态分配的结点
void cleanLinks();

// 输出动态分配内存的次数
void printAllocateTimes();

// 判断内存分配是否失败
void isOutOfMem(void*);

#endif