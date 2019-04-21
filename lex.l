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
#include "parse.h"
#include "y.tab.h"
#include "code_gen.h"
%}

%% 
<<EOF>>                 { return 0; }
\n                      { return 0; }

[ \t\v\n\f]             { /* consume whitespace */ }
"//"[^\n]*              { /* consume //-comment */ }

var                     { return VAR; }
string                  { return STRING; }
bool                    { return BOOL; }

true                    { yylval.boolValue = true; return LITERAL_BOOL; }
false                   { yylval.boolValue = false; return LITERAL_BOOL; }

L?\"(\\.|[^\\"\n])*\"   { yylval.stringValue = strdup(yytext); return LITERAL_STRING; }

{D}+                    { yylval.varValue = atoi(yytext); return LITERAL_NUM; }
({D}+)?"."({D}+)        { yylval.varValue = atof(yytext); return LITERAL_NUM; }

{L}({L}|{D})*           { yylval.stringValue = strdup(yytext); return IDENTIFIER; }

"("                     { return('('); }
")"                     { return(')'); }
"{"                     { return('{'); }
"}"                     { return('}'); }
"="                     { return('='); }
";"                     { return(';'); }
","                     { return(','); }

.                       { 
                            printf("Unrecognized token: %s\n", yytext); 
                            err_in_lex=true; 
                            yyerror("Error during parsing"); 
                        }

%%

int main(int argc, char** args) 
{    
    yyparse();

    if (!err_in_lex && !err_in_parse)
    {
        generate_code(syntax_tree);
    }

    else
    {
        fprintf(stderr, "Error in front-end. Aborting code generation.\n");
    }

    return 0; 
}
