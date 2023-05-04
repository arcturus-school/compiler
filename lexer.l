%option noyywrap

%{

#include "./symtab/symtab.h"
#include "parser.tab.h"

%}

ALPHA         [a-zA-Z]
DIGIT         [0-9]
ALNUM         {ALPHA}|{DIGIT}
INT           "0"|"-"?[1-9]{DIGIT}*
DOU           "0.0"|"-"?{DIGIT}*"."{DIGIT}+
ID            {ALPHA}+{ALNUM}*
CHARACTER     [ -~]
CHAR          \'{CHARACTER}\'
STR           \"{CHARACTER}*\"

%%

"char"        { declare = 1; return CHAR;     }
"int"         { declare = 1; return INTEGER;  }
"double"      { declare = 1; return DOUBLE;   }
"void"        { declare = 1; return VOID;     }

"if"          { return IF;       }
"else"        { return ELSE;     }
"else if"     { return ELIF;     }
"while"       { return WHILE;    }
"for"         { return FOR;      }

"break"       { return BREAK;    }
"continue"    { return CONTINUE; }

"return"      { return RETURN;   }

"+"           { return ADD_OP;   }
"-"           { return SUB_OP;   }
"*"           { return MUL_OP;   }
"/"           { return DIV_OP;   }
"%"           { return MOD_OP;   }

"=="          { return EQ_OP;    }
"!="          { return NEQ_OP;   }

">"           { return GT_OP;    }
"<"           { return LT_OP;    }
">="          { return GE_OP;    }
"<="          { return LE_OP;    }

"&&"          { return AND_OP;   }
"||"          { return OR_OP;    }
"!"           { return NOT_OP;   }

"("           { return LPAREN;   }
")"           { return RPAREN;   }
"]"           { declare = lastDeclare; return RBRACK;                }
"["           { lastDeclare = declare; declare = 0; return LBRACK;   }
"{"           { return LBRACE;   }
"}"           { return RBRACE;   }

";"           { return SEMI;     }
","           { return COMMA;    }
"="           { return ASSIGN; /* 赋值语句 */ }
"&"           { return REFER;  /* 变量引用 */ }

"++"          { return INC_OP;   }
"--"          { return DEC_OP;   }

"//".*        {   /* 匹配注释 */   }
"#include".*  {   /* 预处理命令 */ }

{ID}          { yylval.node = insert(yytext, yylineno); return ID;   }
{DOU}         { yylval.val.dval = atof(yytext); return DVAL;         }
{INT}         { yylval.val.ival = atoi(yytext); return IVAL;         }
{CHAR}        { yylval.val.cval = yytext[1]; return CVAL;            }
{STR}         { yylval.val.sval = malloc(yyleng * sizeof(char)); 
                strcpy(yylval.val.sval, yytext); return STRING;      }

\n            { yylineno += 1;   }

[ \t\r\f]+    { /* 忽略空白字符 */ }

.             { printf("unrecognized character: %s\n", yytext); /* 未被识别的字符 */ }

%%
