CC = gcc
LEX = flex
YACC = bison

all:
	$(LEX) -l -o pcc_scanner.c --yylineno pcc_syntax.l
	$(YACC) -v -d pcc_grammar.y -o pcc_parser.c
	$(CC) -g -ll -o build/pcc tree_handler.c tree_checker.c code_gen.c pcc_scanner.c pcc_parser.c
	cp compiler_util_functions.lua ./build/
clean:
	-rm pcc_scanner.c pcc_parser.h pcc_parser.c pcc_parser.output
	-rm -r build
