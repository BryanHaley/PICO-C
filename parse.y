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
%type<nodeValue> term_expression fact_expression primary declaration
%type<nodeValue> declaration_with_assign

%%
program
    : { create_global_block(); } global_block 
    ;

global_block
    : function_def
    { add_func_def_to_global_block($1); }
    ;

function_def
    : { clear_arg_def_block(); clear_statement_block(); }
    type IDENTIFIER '(' argument_defintion_block ')' '{' statement_block '}' 
    { $$ = create_func_def_node($2, $3, current_arg_def_block, current_statement_block); }
    ;

argument_defintion_block
    : /* empty */
    | argument_defintion_block ',' argument_defintion
    | argument_defintion
    ;

argument_defintion
    : type IDENTIFIER 
    { $$ = create_arg_def_node($1, $2); }
    ;

statement_block
    : /* empty */
    | statement_block statement
    ;

statement
    : ';'
    { handle_null_stmnt_node(); }
    | assignment ';'
    { handle_stmnt_node($1); }
    | function_call ';'
    { handle_stmnt_node($1); }
    | declaration ';'
    { handle_stmnt_node($1); }
    | declaration_with_assign ';'
    { handle_stmnt_node($1); }
    ;

assignment
    : IDENTIFIER '=' expression
    { $$ = create_assign_node($1, $3); }
    ;

function_call
    : IDENTIFIER '(' { clear_arg_block(); } argument_block ')' 
    { $$ = create_func_call_node($1, current_arg_block); }
    ;

argument_block
    :   /* empty */
    | argument_block ',' argument
    | argument
    ;

argument
    : expression
    { handle_arg_node($1); }
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
    : '(' expression ')'
    { $$ = $2; }
    | fact_expression
    { $$ = $1; }
    | term_expression
    { $$ = $1; }
    | primary
    { $$ = $1; }
    ;

fact_expression
    : expression '*' expression
    { $$ = create_bin_expr_node($1, $3, '*'); }
    | expression '/' expression
    { $$ = create_bin_expr_node($1, $3, '/'); }
    ;

term_expression
    : expression '+' expression
    { $$ = create_bin_expr_node($1, $3, '+'); }
    | expression '-' expression
    { $$ = create_bin_expr_node($1, $3, '-'); }
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
