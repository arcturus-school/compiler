CFLAGS=`pkg-config libgvc --cflags` -g -O2 # 给编译器的编译选项 
LDLIBS=`pkg-config libgvc --libs` # 连接库选项

GRAPH_H = ./ast/graph/graph.h
GRAPH_C = ./ast/graph/graph.c
AST_H = ./ast/ast.h ./utils/types.h
AST_C = ./ast/ast.c
CHECK_H = ./semantics/semantics.h
CHECK_C = ./semantics/semantics.c
SYMTAB_H = ./symtab/symtab.h
SYMTAB_C = ./symtab/symtab.c
COMMON_C = ./utils/common.c
COMMON_H = ./utils/common.h
INTER_H = ./intermediate/intermediate.h
INTER_C = ./intermediate/intermediate.c

clean:
	rm -f *.o *.out
	rm *.tab.c *.tab.h *.yy.c

compile: lexer.l parser.y $(AST_H) $(AST_C) $(GRAPH_H) $(GRAPH_C) $(CHECK_H) $(CHECK_C) $(SYMTAB_H) $(SYMTAB_C) $(COMMON_H) $(COMMON_C) $(INTER_H) $(INTER_C)
		bison -d parser.y -Wcounterexamples
		flex lexer.l
		cc -o $@ parser.tab.c $(AST_C) $(GRAPH_C) $(CHECK_C) $(SYMTAB_C) $(COMMON_C) $(INTER_C) lex.yy.c $(CFLAGS) $(LDLIBS)
		make clean

build:
		./compile ./test/array.c
		./compile ./test/case1.c
		./compile ./test/case2.c
		./compile ./test/func.c
		./compile ./test/pointer.c