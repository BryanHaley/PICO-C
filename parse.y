%token VAR STRING BOOL IDENTIFIER LITERAL_NUM LITERAL_STRING LITERAL_BOOL

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"
#include "ast.h"
#include "tree_handler.h"
%}

%union
{
    double  varValue;
    char*   stringValue;
    bool    boolValue;
    node_t* nodeValue;
}

%type<stringValue> IDENTIFIER LITERAL_STRING type
%type<varValue> LITERAL_NUM
%type<boolValue> LITERAL_BOOL
%type<nodeValue> function_def function_call argument_defintion assignment expression
%type<nodeValue> primary declaration declaration_with_assign statement argument
%type<nodeValue> argument_definition_block statement_block argument_block

%left '+' '-'
%left '*' '/'

%%
program
    : { create_global_block(); } global_block 
    ;

global_block
    : function_def
    { add_func_def_to_global_block($1); }
    ;

function_def
    : type IDENTIFIER '(' argument_definition_block ')' '{' statement_block '}' 
    { $$ = create_func_def_node($1, $2, $4, $7); }
    ;

argument_definition_block
    : argument_definition_block argument_defintion
    { $$ = handle_parent_block($1, NODE_ARG_DEF_BLOCK, $2); }
    | /* empty */
    { $$ = NULL; }
    ;

argument_defintion
    : type IDENTIFIER ',' 
    { $$ = create_arg_def_node($1, $2); }
    | type IDENTIFIER
    { $$ = create_arg_def_node($1, $2); }
    ;

statement_block
    : statement_block statement
    { $$ = handle_parent_block($1, NODE_STMNT_BLOCK, $2); }
    | /* empty */
    { $$ = NULL; }
    ;

statement
    : function_call ';'
    { $$ = $1; }
    | declaration_with_assign ';'
    { $$ = $1; }
    | declaration ';'
    { $$ = $1; }
    | assignment ';'
    { $$ = $1; }
    | ';'
    { $$ = NULL; }
    ;

assignment
    : IDENTIFIER '=' expression
    { $$ = create_assign_node($1, $3); }
    ;

function_call
    : IDENTIFIER '(' argument_block ')' 
    { $$ = create_func_call_node($1, $3); }
    ;

argument_block
    : argument_block argument
    { $$ = handle_parent_block($1, NODE_ARG_BLOCK, $2); }
    | /* empty */
    { $$ = NULL; }
    ;

argument
    : expression ','
    { $$ = $1; }
    | expression
    { $$ = $1; }
    ;

declaration
    : type IDENTIFIER
    { $$ = create_declaration_node($1, $2); }
    ;

declaration_with_assign
    : type IDENTIFIER '=' expression
    { $$ = create_declaration_with_assign_node($1, $2, $4); }
    ;

expression
    : expression '+' expression
    { $$ = create_bin_expr_node($1, $3, '+'); }
    | expression '-' expression
    { $$ = create_bin_expr_node($1, $3, '-'); }
    | expression '*' expression
    { $$ = create_bin_expr_node($1, $3, '*'); }
    | expression '/' expression
    { $$ = create_bin_expr_node($1, $3, '/'); }
    | '(' expression ')'
    { 
        bin_expr_data* data = (bin_expr_data*) $2->data;
        data->in_parentheses = true;
        $$ = $2; 
    }
    | primary
    { $$ = $1; }
    ;

primary
    : LITERAL_NUM
    { $$ = create_primary_node_num(PRI_LITERAL_NUM, $1); }
    | LITERAL_STRING
    { $$ = create_primary_node_str(PRI_LITERAL_STR, $1); }
    | IDENTIFIER
    { $$ = create_primary_node_str(PRI_IDENTIFIER, $1); }
    | function_call
    { $$ = create_primary_node_nde(PRI_FUNC_CALL, $1); }
    ;

type
    : VAR { $$ = "var"; }
    | BOOL { $$ = "bool"; }
    | STRING { $$ = "string"; }
    | IDENTIFIER { $$ = $1; }
    ;
%%

void yyerror (char *s) 
{
   fprintf (stderr, "%s\n", s);

   err_in_parse = true;
}
