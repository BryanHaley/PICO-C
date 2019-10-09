#ifndef PCC_GRAMMAR_H
#define PCC_GRAMMAR_H

#include <stdbool.h>

int yylineno;

int yylex(void);
int yyparse(void);
void yyerror(const char* s);

bool err_in_lex;
bool err_in_parse;
bool err_in_tree;
bool err_in_code_gen;

#endif
