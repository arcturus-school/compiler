%{

#include "./ast/ast.h"
#include "./symtab/symtab.h"
#include "./semantics/semantics.h"
#include "./intermediate/intermediate.h"

extern int yyparse();
extern int yylex();
extern void yyerror();

extern FILE* yyin;   // 文件流

char* fileName        = NULL;   // 图片名
AstFuncDeclNode* func = NULL;   // 当前函数
int valueCount        = 0;      // 数组初始化数据的个数
int pointerCount      = 0;      // 几级指针

%}

%union {
  AstNode*   node;
  valueTypes val;
  int        dataType;
}

%token <val>  DVAL
%token <val>  IVAL
%token <val>  CVAL
%token <val>  STRING
%token <node> ID

/* data type */
%token <dataType> DOUBLE INTEGER VOID CHAR
%type  <dataType> type

/* ( ) [ ] { } ; , = & */
%token <val> LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE SEMI COMMA ASSIGN REFER

/* operator */
%token <val> ADD_OP SUB_OP MUL_OP DIV_OP MOD_OP 
%token <val> EQ_OP NEQ_OP
%token <val> GT_OP LT_OP GE_OP LE_OP
%token <val> AND_OP OR_OP NOT_OP
%token <val> INC_OP DEC_OP
%type  <val> pointer

%token <val> CONTINUE BREAK RETURN

/* statement */
%token <val> IF ELSE WHILE FOR ELIF

/* non-terminal */
%type <node> program opt_blocks blocks block body
%type <node> variables vardecl var_ref const consts
%type <node> variable init
%type <node> expression assigment
%type <node> statement statements opt_stmt
%type <node> while if else elif opt_elif for
%type <node> function return args opt_args params param opt_params function_call
%type <node> array

%left  COMMA
%right ASSIGN
%left  OR_OP
%left  AND_OP
%left  EQ_OP NEQ_OP
%left  GT_OP LT_OP GE_OP LE_OP
%left  ADD_OP SUB_OP
%left  MUL_OP DIV_OP MOD_OP
%right NOT_OP INC_OP DEC_OP REFER
%left  LPAREN RPAREN LBRACK RBRACK

%start program

%%
program : opt_blocks 
        { 
            $$ = newAstRootNode($1);
            createImage($$, fileName);             // 绘制语法树
            createIntermediateCode($$, fileName);  // 生成中间代码
            cleanLinks();                          // 清理语法树
            printAllocateTimes();                  // 显示程序动态分配内存次数
        }
        ;

opt_blocks : blocks          { $$ = $1;   }
           | /* empty */     { $$ = NULL; }
           ;

blocks : blocks block  { AstNode* t = getLastSiblingNode($1); t->next = $2; $$ = $1; }
       | block         { $$ = $1;                                                    }
       ;

block : vardecl        { $$ = $1; /* 全局变量声明 */ }
      | function       { $$ = $1;                  }
      ;

opt_stmt : statements  { $$ = $1;                  }
         | /* empty */ { $$ = NULL;                }
         ;

statements : statements statement
           {
               AstNode* t = getLastSiblingNode($1);
               if (t != NULL) {
                   // 避免 return 语句后面的语句被加进来
                   t->next = $2; 
               }

               $$ = $1;
           }
           | statement                 { $$ = $1;                   }
           ;

statement : if                         { $$ = $1;                   }
          | for                        { $$ = $1;                   }
          | while                      { $$ = $1;                   }
          | vardecl                    { $$ = $1;                   }
          | expression SEMI            { $$ = $1;                   }
          | return                     { $$ = $1;                   }
          | CONTINUE SEMI              { $$ = newAstContinueNode(); }
          | BREAK SEMI                 { $$ = newAstBreakNode();    }
          ;

if : IF LPAREN expression RPAREN { incrScope(); } body { hideScope(); } opt_elif else
   { 
      AstNode* t = newAstIfBranchNode($3, $6);
      t->next = $8;
      AstNode* n = getLastSiblingNode(t);
      n->next = $9;
      $$ = newAstIfNode(t); 
   }
   ;

opt_elif : elif          { $$ = $1; }
         | /* empty */   { $$ = NULL; }
         ;

elif : ELIF LPAREN expression RPAREN { incrScope(); } body
     {   
        hideScope(); 
        $$ = newAstIfBranchNode($3, $6); 
     }
     | elif ELIF LPAREN expression RPAREN { incrScope(); } body  
     { 
        hideScope(); 
        AstNode* n = getLastSiblingNode($1);
        n->next = newAstIfBranchNode($4, $7);
        $$ = $1;
     }
     ;

else : ELSE { incrScope(); } body  { hideScope(); $$ = newAstIfBranchNode(NULL, $3); /* else 语句 */ }
     | /* empty */                 { $$ = NULL; }
     ;

for : { incrScope(); } FOR LPAREN vardecl expression SEMI expression RPAREN body
    { 
        // for 语句
        hideScope(); 
        $$ = newAstForNode($4, $5, $7, $9);
    }
    ;

while : WHILE LPAREN expression RPAREN { incrScope(); } body
      { 
         // while 语句
         hideScope();
         $$ = newAstWhileNode($3, $6);
      }
      ;

expression : expression ADD_OP expression  { $$ = newAstArithmNode(ADD, $1, $3);        }
           | expression SUB_OP expression  { $$ = newAstArithmNode(SUB, $1, $3);        } 
           | expression MUL_OP expression  { $$ = newAstArithmNode(MUL, $1, $3);        }
           | expression DIV_OP expression  { $$ = newAstArithmNode(DIV, $1, $3);        }
           | expression MOD_OP expression  { $$ = newAstArithmNode(MOD, $1, $3);        }
           | ID INC_OP                     { $$ = newAstIncNode((AstRefNode*)$1, 0); }
           | ID DEC_OP                     { $$ = newAstDecNode((AstRefNode*)$1, 0); }
           | INC_OP ID                     { $$ = newAstIncNode((AstRefNode*)$2, 1); }
           | DEC_OP ID                     { $$ = newAstIncNode((AstRefNode*)$2, 1); }
           | expression OR_OP expression   { $$ = newAstBoolNode(OR, $1, $3);           }
           | expression AND_OP expression  { $$ = newAstBoolNode(AND, $1, $3);          }
           | NOT_OP expression             { $$ = newAstBoolNode(NOT, $2, NULL);        }
           | expression EQ_OP expression   { $$ = newAstEquNode(EQ, $1, $3);            }
           | expression NEQ_OP expression  { $$ = newAstEquNode(NEQ, $1, $3);           }
           | expression GE_OP expression   { $$ = newAstRelNode(GE, $1, $3);            }      
           | expression GT_OP expression   { $$ = newAstRelNode(GT, $1, $3);            }      
           | expression LE_OP expression   { $$ = newAstRelNode(LE, $1, $3);            }      
           | expression LT_OP expression   { $$ = newAstRelNode(LT, $1, $3);            }      
           | LPAREN expression RPAREN      { $$ = $2;                                   }
           | function_call                 { $$ = $1;                                   }
           | const                         { $$ = $1;                                   }
           | var_ref                       { $$ = $1;                                   }
           | assigment                     { $$ = $1;                                   }
           ;

assigment : var_ref ASSIGN expression
          { 
            checkAssign($1, $3); // 检查赋值是否合理
            $$ = newAstAssignNode($1, $3); // 变量赋值  
          };

function : funcdecl body
         { 
            hideScope();
            func->body = $2;
            $$ = (AstNode*)func;
         }
         ;

funcdecl : type variable { declare = 0; incrScope(); } LPAREN opt_params
         {
            AstSymbolNode* temp = (AstSymbolNode*)$2;
            temp->type = $1;
            temp->nodeType = FUNC_NODE;
            func = (AstFuncDeclNode*)newAstFuncDeclNode($2, $5, NULL);
         } RPAREN
         ;

opt_params : params          { $$ = $1;   }
           | /* empty */     { $$ = NULL; /* 参数可为空 */ }
           ;

params : params COMMA param
       {    
            AstNode* t = getLastSiblingNode($1);
            t->next = $3; 
            $$ = $1; 
       }
       | param               { $$ = $1;   }
       ;

param : type ID              
      { 
            declare = 0;
            AstSymbolNode* temp = (AstSymbolNode*)$2;
            temp->type = $1;
            temp->id = nid++;
            $$ = newAstDeclNode($2); /* 参数声明 */ 
      }
      | type pointer ID
      { 
            declare = 0; 
            AstSymbolNode* temp = (AstSymbolNode*)$3;
            temp->type = $1; 
            temp->id = nid++; 
            temp->pointer = pointerCount; 
            $$ = newAstDeclNode($3); /* 带指针的参数 */ 
      }
      ;

function_call : ID LPAREN opt_args RPAREN { $$ = newAstFuncCallNode($1, $3); /* 函数调用 */ }
              ;

opt_args : args              { $$ = $1;   }
         | /* empty */       { $$ = NULL; } 
         ;

args : args COMMA expression
     {
            AstNode* t = getLastSiblingNode($1);
            t->next = $3; 
            $$ = $1; 
     }
     | expression { $$ = $1; }          
     ;

vardecl : type variables { declare = 0; } SEMI
        {  
            giveNodeType($2, $1);
            checkAstNodeType($2); /* 此时 next 后全是 decl 结点, 但 semi 后就不一定了 */
            $$ = $2;
        }
        ;

variables : variables COMMA variable
          {    
                AstNode* t = getLastSiblingNode($1); 
                t->next = newAstDeclNode($3); 
          }
          | variables COMMA init
          { 
                AstNode* t = getLastSiblingNode($1);
                t->next = newAstDeclNode($3); 
          }
          | variable  { $$ = newAstDeclNode($1); }
          | init      { $$ = newAstDeclNode($1); }
          ;

variable : ID { $$ = $1; /* 变量名 */ }
         | pointer ID
         {
            if (declare) {
                ((AstSymbolNode*)$2)->pointer = pointerCount; /* 指针 */
                $$ = $2;
            } else {
                valueTypes v;
                v.ival = pointerCount;
                ((AstRefNode*)$2)->indicate = newAstConstNode(INT_TYPE, v);
                ((AstRefNode*)$2)->nodeType = GETTER_NODE; /* 取地址操作 */
                $$ = $2;
            }
         }
         | ID array 
         {
            if (declare) {
               AstSymbolNode* temp = (AstSymbolNode*)$1;
               temp->nodeType = ARRAY_NODE; /* 声明一个数组 */
               checkArraySize(temp, $2, 0); /* 检测数组大小是否合规 */
               temp->size = ((AstConstNode*)$2)->value.ival;
               $$ = $1;
            } else {
               AstRefNode* temp = (AstRefNode*)$1;
               checkIndexType(temp, $2);
               temp->indicate = $2;
               temp->nodeType = ARRAYITEM_NODE; /* 数组调用 */
               $$ = $1;
            }  
         }
         ;

init : ID { declare = 0; /* 防止 expression 被认为是重复声明 */ } ASSIGN expression
     { 
        // 变量初始化
        ((AstSymbolNode*)$1)->value = $4; 
        $$ = $1;
        declare = 1; // 可能还有其他的变量要声明, 所以先恢复一下
     }
     | pointer ID { declare = 0; } ASSIGN expression
     {
        // 指针数据初始化
        AstSymbolNode* temp = (AstSymbolNode*)$2;
        temp->value = $5;
        temp->pointer = pointerCount;
        $$ = $2;
        declare = 1;
     }
     | ID array { declare = 0; valueCount = 0; } ASSIGN LBRACE consts RBRACE
     {
        // 数组初始化
        AstSymbolNode* temp = (AstSymbolNode*)$1;
        temp->nodeType = ARRAY_NODE;
        checkArraySize(temp, $2, valueCount);

        if ($2 == NULL) {
            temp->size = valueCount;
        } else {
            temp->size = ((AstConstNode*)$2)->value.ival;
        }

        temp->value = $6;
        
        $$ = $1;
        declare = 1;
     }
     ;

array : LBRACK ID RBRACK         { $$ = $2;   }
      | LBRACK const RBRACK      { $$ = $2;   }
      | LBRACK RBRACK            { $$ = NULL; }
      ;

var_ref : variable        { $$ = $1; /* 普通变量 */ }
        | REFER variable
        { 
            AstRefNode* temp = (AstRefNode*)$2;
            temp->nodeType = ADDRESS_NODE;
            $$ = $2; /* 引用变量 */
        }
        ;

const : IVAL   { $$ = newAstConstNode(INT_TYPE, $1);     }
      | DVAL   { $$ = newAstConstNode(DOUBLE_TYPE, $1);  }
      | CVAL   { $$ = newAstConstNode(CHAR_TYPE, $1);    }
      | STRING { $$ = newAstConstNode(STRING_TYPE, $1);  }
      ;

consts : consts COMMA const 
       { 
           // 用于数组初始化 
           valueCount++;
           AstNode* t = getLastSiblingNode($1);
           t->next = $3; 
           $$ = $1;
       }
       | const                { $$ = $1; valueCount = 1; }
       ;

body : LBRACE opt_stmt RBRACE { $$ = $2;                 }
     ;

type : INTEGER                { $$ = INT_TYPE;           }
     | DOUBLE                 { $$ = DOUBLE_TYPE;        }
     | VOID                   { $$ = VOID_TYPE;          }
     | CHAR                   { $$ = CHAR_TYPE;          }
     ;

return : RETURN expression SEMI
       {  
            $$ = newAstReturnNode(func->function->type, $2); 
            checkFunctionReturn(func, (AstReturnNode*)$$); /* 检测返回是否符合函数定义 */ 
       }
       | RETURN SEMI
       { 
            $$ = newAstReturnNode(func->function->type, NULL); 
            checkFunctionReturn(func, (AstReturnNode*)$$);
       }
       ;

pointer : MUL_OP         { pointerCount = 1; /* 一级指针 */ }
        | pointer MUL_OP { pointerCount++; /* 多级指针 */   }
        ;
%%

int main(int argc, char** argv) {
    fileName = argv[1];

    if (NULL == (yyin = fopen(fileName, "r"))) {
        showErrorInfo("open file %s failed.\n", fileName);
    }

    initSymbolTable(); // 初始化符号表

    // 查看文件内容
    // while(1) {
    //   int c = fgetc(yyin);
    //   if (feof(yyin)) { 
    //       break ;
    //   }
    //   printf("%c", c);
    // }
    // printf("\n");

    int flag = yyparse();

    fclose(yyin);

    return flag;
}

void yyerror() {
    printf("error in line %d.\n", yylineno);
}
