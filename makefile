all:
	lex lex.l
	yacc -d parse.y
	gcc -g -ll -o build/test tree_handler.c lex.yy.c y.tab.c
clean:
	-rm y.tab.c y.tab.h lex.yy.c
	-rm -r build
	-rm -r out
	mkdir build
	mkdir out
