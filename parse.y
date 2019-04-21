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
    double varValue;
    char *stringValue;
    bool boolValue;
    void *nodeValue;
}

%type<stringValue> IDENTIFIER LITERAL_STRING
%type<varValue> LITERAL_NUM
%type<boolValue> LITERAL_BOOL
%type<nodeValue> function_def argument_defintion function_call

%%
program
    : /* empty */
    | { handle_global_block(); } global_block 
    ;

global_block
    : function_def
    ;

function_def
    : { current_func_def = create_node(syntax_tree->global_block, NODE_FUNC_DEF); }
      { handle_arg_def_block(); handle_statement_block(); }
    VAR IDENTIFIER '(' argument_defintion_block ')' '{' statement_block '}' 
    {
        handle_function_def($4, TYPE_VAR);
    }
    | { free(current_func_def); free(current_arg_block); free(current_statement_block); }
      { current_func_def = create_node(syntax_tree->global_block, NODE_FUNC_DEF); }
      { handle_arg_def_block(); handle_statement_block(); }
    BOOL IDENTIFIER '(' argument_defintion_block ')' '{' statement_block '}' 
    {
        handle_function_def($5, TYPE_BOOL);
    }
    | { free(current_func_def); free(current_arg_block); free(current_statement_block); }
      { current_func_def = create_node(syntax_tree->global_block, NODE_FUNC_DEF); }
      { handle_arg_def_block(); handle_statement_block(); }
    STRING IDENTIFIER '(' argument_defintion_block ')' '{' statement_block '}' 
    {
        handle_function_def($5, TYPE_STRING);
    }
    ;

function_call
    : { handle_arg_block(); }
    IDENTIFIER '(' argument_block ')' 
    { 
        handle_func_call(get_pointer_symbol(current_scope, $2, false, false));
    }
    ;

argument_defintion_block
    : /* empty */
    | argument_defintion_block ',' argument_defintion
    | argument_defintion
    ;

argument_defintion
    : VAR IDENTIFIER 
    { handle_arg_def(TYPE_VAR, $2); }
    | BOOL IDENTIFIER
    { handle_arg_def(TYPE_BOOL, $2); }
    | STRING IDENTIFIER
    { handle_arg_def(TYPE_STRING, $2); }
    ;

statement_block
    : /* empty */
    | statement_block statement
    ;

statement
    : ';'
    | assignment ';'
    | function_call ';'
    ;

assignment
    : IDENTIFIER '=' IDENTIFIER 
    { handle_assignment_to_identifier($1, $3); }
    | IDENTIFIER '=' LITERAL_NUM 
    {
        symbol_value val = { $3 };
        handle_assignment_to_literal($1, TYPE_LITERAL_NUM, val); 
    }
    ;

argument_block
    :   /* empty */
    | argument_block ',' argument
    | argument
    ;

argument
    : IDENTIFIER 
    {
        node_t *arg = create_node(current_arg_block, NODE_ARG);
        ((arg_data*) arg->data)->identifier = 
            get_pointer_symbol(current_scope, $1, false, false);
        
        node_t* child[1] = { arg };
        add_child_to_parent_block(current_arg_block, 1, child);
    }
    | LITERAL_NUM 
    {
        node_t *arg = create_node(current_arg_block, NODE_ARG);
        ((arg_data*) arg->data)->identifier = get_literal_num_symbol($1);
        
        node_t* child[1] = { arg };
        add_child_to_parent_block(current_arg_block, 1, child);
    }
    ;
%%

void yyerror (char *s) 
{
   fprintf (stderr, "%s\n", s);

   err_in_parse = true;
}
