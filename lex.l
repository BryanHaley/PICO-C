D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P           ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS          ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "y.tab.h"

int yyparse();
%}

%% 
<<EOF>>			{ return 0; }
\n				{ return 0; }

[ \t\v\n\f]		{ /* consume whitespace */ }
"//"[^\n]*      { /* consume //-comment */ }

var 		  	{ printf("Lex found keyword: var\n"); yylval.stringValue = strdup(yytext); return VAR; }

{L}({L}|{D})* 	{ printf("Lex found identifier: %s\n", yytext); yylval.stringValue = strdup(yytext); return IDENTIFIER; }

"("				{ printf("Lex found token: (\n"); return('('); }
")"				{ printf("Lex found token: )\n"); return(')'); }
"{"				{ printf("Lex found token: {\n"); return('{'); }
"}"				{ printf("Lex found token: }\n"); return('}'); }
"="				{ printf("Lex found token: =\n"); return('='); }
";"				{ printf("Lex found token: ;\n"); return(';'); }
","				{ printf("Lex found token: ,\n"); return(','); }


{D}+				{ printf("Lex found literal number: %s\n", yytext); yylval.varValue = atoi(yytext); return LITERAL_NUM; }
({D}+)?"."({D}+)	{ printf("Lex found literal number: %s\n", yytext); yylval.varValue = atof(yytext); return LITERAL_NUM; }

.				{ printf("Unrecognized token: %s\n", yytext); return 0; }

%%

int main(int argc, char** args) {
    
    //while (yylex() != 0) { }
    yyparse();
    return 0; 
}