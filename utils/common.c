#include "common.h"

char* dt[] = {
    "int",
    "double",
    "void",
    "undefined",
    "char",
    "char",
};

char* arithm[] = {
    "+",
    "-",
    "*",
    "%",
    "/",
};

char* bool[] = {
    "||",
    "&&",
    "!",
};

char* rel[] = {
    ">",
    "<",
    ">=",
    "<=",
};

char* equ[] = {
    "==",
    "!=",
};

static NodeLink* nodeLink = NULL;
static int       times    = 0;

void showErrorInfo(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, ERROR, "error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void showWaringInfo(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, WARNING, "warning: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

char* getFileName(char* fileName, char* base, char* ext) {
    int baseLen = strlen(base);
    int extLen  = strlen(ext);

    char* temp = (char*)malloc(sizeof(char) * (strlen(fileName) + 1));
    strcpy(temp, fileName);

    char* pre = NULL;

    // 找到文件名
    char* token = strtok(temp, "/");

    while (token != NULL) {
        pre   = token;
        token = strtok(NULL, "/");
    }

    // 去掉文件格式
    int idx = strlen(pre) - 1;

    while (idx >= 0) {
        if (pre[idx] == '.') break;
        idx--;
    }

    char* res = (char*)malloc(sizeof(char) * (baseLen + extLen + idx + 1));
    strcpy(res, base);
    strncat(res, pre, idx);
    strcat(res, ext);

    free(temp);

    return res;
}

void addToLink(void* node) {
    NodeLink* t = (NodeLink*)malloc(sizeof(NodeLink));
    t->node     = node;
    t->next     = nodeLink;
    nodeLink    = t;
    times++;
}

void printAllocateTimes() {
    printf("dynamically allocate memory %d times\n", times);
}

void cleanLinks() {
    NodeLink *cur = nodeLink, *next = NULL;

    while (cur != NULL) {
        next = cur->next;
        free(cur->node);
        free(cur);
        cur = next;
    }
}

void isOutOfMem(void* n) {
    if (NULL == n) {
        showErrorInfo("out of memory.\n");
    }
}