### 关于语法树清理工作

一开始想着使用递归进行清理, 即从根节点出发, 然后调用 treefree 进行递归清理

没有使用符号表的时候, 这个方法没啥毛病

但是使用符号表后, 不同的抽象语法树结点就会共用一些结点, 这就导致了一个结点被 free 了, 后来想要清理的时候就会报 `double free or corruption (out)` 的错误, 也就是一个结点不能被 free 两次

```c
// 原来的代码
void freeSymbol(Symbol* s) {
    if (s) {
        if (s->nodeType == ARRAYITEM_NODE) {
            free(s);
        } else {
            free(s->s_name);

            if (s->s_value) {
                treefree((AstNode*)s->s_value);
            }

            // 清空引用链
            RefList *cur = s->lines, *next = NULL;

            while (cur != NULL) {
                next = cur->next;
                free(cur);
                cur = next;
            }

            free(s);
        }
    }
}

void freeConstNode(ConstNode* node) {
    if (node->dataType == STRING_TYPE) {
        free(node->value.sval);
    }

    free(node);
}

void treefree(AstNode* node) {
    if (!node) return;

    switch (node->nodeType) {
        case ROOT_NODE: {
            treefree(node->next);
            break;
        }
        case FUNC_DEF: {
            AstFuncDeclNode* n = (AstFuncDeclNode*)node;

            treefree(n->next);
            treefree(n->param);
            treefree(n->body);
            treefree(n->ret);
            freeSymbol(n->function);

            break;
        }
        case CONST_NODE: {
            freeConstNode((ConstNode*)node);
            return;
        }
        case REF_NODE: {
            AstRefNode* n = (AstRefNode*)node;

            freeSymbol(n->variable);
            treefree(n->next);
            break;
        }
        case VARIABLE_NODE: {
            freeSymbol((Symbol*)node);
            break;
        }
        case RETURN_NODE: {
            AstReturnNode* n = (AstReturnNode*)node;
            // return 不会再有 next, 所以无需清理
            treefree(n->expression);
            break;
        }
        case STATEMENT_NODE: {
            AstStatementNode* n = (AstStatementNode*)node;

            switch (n->statementType) {
                case CONTINUE_STMT:
                case BREAK_STMT: {
                    treefree(n->next);
                    break;
                }
                case FOR_STMT: {
                    AstForNode* m = (AstForNode*)n;

                    treefree(m->next);
                    treefree(m->initialization);
                    treefree(m->condition);
                    treefree(m->end);
                    treefree(m->body);

                    break;
                }
                case WHILE_STMT: {
                    AstWhileNode* m = (AstWhileNode*)n;

                    treefree(m->next);
                    treefree(m->condition);
                    treefree(m->body);

                    break;
                }
                case IF_STMT: {
                    AstIfNode* m = (AstIfNode*)n;

                    treefree(m->next);
                    treefree(m->branch);

                    break;
                }
                case IF_BRANCH_STMT: {
                    AstIfBranchNode* m = (AstIfBranchNode*)n;

                    treefree(m->next);
                    treefree(m->condition);
                    treefree(m->body);

                    break;
                }
                case DECL_STMT: {
                    AstDeclNode* m = (AstDeclNode*)n;

                    treefree(m->next);
                    freeSymbol(m->variable);

                    break;
                }
                case EXPRESSION_STMT: {
                    AstExpressionNode* e = (AstExpressionNode*)n;

                    switch (e->expressionType) {
                        case FUNC_CALL_EXP: {
                            AstFuncCallNode* m = (AstFuncCallNode*)n;

                            treefree(m->next);
                            treefree(m->argument);
                            freeSymbol(m->function);

                            break;
                        }
                        case INC_EXP: {
                            AstIncNode* d = (AstIncNode*)e;

                            treefree(d->next);
                            freeSymbol(d->variable);

                            break;
                        }
                        case DEC_EXP: {
                            AstDecNode* d = (AstDecNode*)e;

                            treefree(d->next);
                            freeSymbol(d->variable);

                            break;
                        }
                        case ASSIGN_EXP:
                        case ARITHM_EXP:
                        case BOOL_EXP:
                        case REL_EXP:
                        case EQU_EXP: {
                            // 确保这五个结构体前几个数据大小一致即可
                            AstAssignNode* a = (AstAssignNode*)e;
                            treefree(a->next);
                            treefree(a->left);
                            treefree(a->right);
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case ARRAYITEM_NODE:
            break;
    }

    free(node);
}
```

后来觉得这种方案不仅麻烦, 每次添加一种类型的结点时就要考虑各自分支, 有可能还有忘记 free 的结点, 因此后来转变思想, 使用了一个链表收集动态分配的结点, 这样使用一个循环即可全部 free 掉, 虽然浪费了一些空间, 但是效率提高了不少, 并且不会存在漏 free 的结点!