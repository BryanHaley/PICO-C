#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>

int yylex(void);
int yyparse(void);
void yyerror(char* s);

bool err_in_lex;
bool err_in_parse;

#endif
