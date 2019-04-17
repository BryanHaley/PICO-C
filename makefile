all:
	lex lex.l
	yacc -d parse.y
	gcc -g -ll -o build/test tree_handler.c lex.yy.c y.tab.c
