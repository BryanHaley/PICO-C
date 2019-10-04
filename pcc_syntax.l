D   [0-9]
L   [a-zA-Z_]
H   [a-fA-F0-9]
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h"
#include "pcc_grammar.h"
#include "pcc_parser.h"
%}

%% 
<<EOF>>                 { return 0; }

[ \t\v\n\f]             { /* consume whitespace */ }
"//"[^\n]*              { /* consume //-comment */ }

var                     { return VAR; }
Var                     { return VAR; }
VAR                     { return VAR; }
string                  { return STRING; }
String                  { return STRING; }
STRING                  { return STRING; }
bool                    { return BOOL; }
Bool                    { return BOOL; }
BOOL                    { return BOOL; }
boolean                 { return BOOL; }
Boolean                 { return BOOL; }
BOOLEAN                 { return BOOL; }
array                   { return ARRAY; }
Array                   { return ARRAY; }
ARRAY                   { return ARRAY; }
arr                     { return ARRAY; }
Arr                     { return ARRAY; }
ARR                     { return ARRAY; }
struct                  { return STRUCT; }
Struct                  { return STRUCT; }
STRUCT                  { return STRUCT; }

global                  { return GLOBAL; }
Global                  { return GLOBAL; }
GLOBAL                  { return GLOBAL; }

length                  { return LENGTH; }
Length                  { return LENGTH; }
LENGTH                  { return LENGTH; }

new                     { return NEW; }
New                     { return NEW; }
NEW                     { return NEW; }

true                    { yylval.boolValue = true; return LITERAL_BOOL; }
True                    { yylval.boolValue = true; return LITERAL_BOOL; }
TRUE                    { yylval.boolValue = true; return LITERAL_BOOL; }
false                   { yylval.boolValue = false; return LITERAL_BOOL; }
False                   { yylval.boolValue = false; return LITERAL_BOOL; }
FALSE                   { yylval.boolValue = false; return LITERAL_BOOL; }

L?\"(\\.|[^\\"\n])*\"   { yylval.stringValue = strdup(yytext); return LITERAL_STRING; }

{D}+                    { yylval.varValue = atoi(yytext); return LITERAL_NUM; }
({D}+)?"."({D}+)        { yylval.varValue = atof(yytext); return LITERAL_NUM; }

{L}({L}|{D})*           { yylval.stringValue = strdup(yytext); return IDENTIFIER; }

">>"                    { return RIGHT_SHIFT; }
"<<"                    { return LEFT_SHIFT; }
"=="                    { return EQUAL_EQUAL; }
"!="                    { return NOT_EQUAL; }
"+="                    { return PLUS_EQUAL; }
"-="                    { return MINUS_EQUAL; }
"*="                    { return TIMES_EQUAL; }
"/="                    { return DIVIDE_EQUAL; }
"%="                    { return MODULO_EQUAL; }
">>="                   { return RIGHT_SHIFT_EQUAL; }
"<<="                   { return LEFT_SHIFT_EQUAL; }
"&="                    { return AND_EQUAL; }
"|="                    { return OR_EQUAL; }
"^="                    { return XOR_EQUAL; }
"++"                    { return PLUS_PLUS; }
"--"                    { return MINUS_MINUS; }
"&&"                    { return LOGICAL_AND; }
"||"                    { return LOGICAL_OR; }
"<="                    { return LESS_THAN_OR_EQUAL; }
">="                    { return GREATER_THAN_OR_EQUAL; }
"="                     { return('=');  }
"("                     { return('(');  }
")"                     { return(')');  }
"{"                     { return('{');  }
"}"                     { return('}');  }
";"                     { return(';');  }
","                     { return(',');  }
"+"                     { return('+');  }
"-"                     { return('-');  }
"*"                     { return('*');  }
"/"                     { return('/');  }
"%"                     { return('%');  }
"&"                     { return('&');  }
"|"                     { return('|');  }
"^"                     { return('^');  }
"<"                     { return('<');  }
">"                     { return('>');  }
"["                     { return('[');  }
"]"                     { return(']');  }
"."                     { return('.');  }

.                       { 
                            printf("Unrecognized token: %s\n", yytext); 
                            err_in_lex=true; 
                            yyerror("Error during parsing"); 
                        }

%%
