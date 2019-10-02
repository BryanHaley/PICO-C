all:
	lex -l lex.l
	yacc -v -d parse.y
	gcc -g -ll -o build/pcc tree_handler.c code_gen.c lex.yy.c y.tab.c
	cp compiler-util-functions.lua ./build/
clean:
	-rm y.tab.c y.tab.h lex.yy.c y.output
	-rm -r build
	-rm -r out
	mkdir build
	mkdir out
