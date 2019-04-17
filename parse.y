%token VAR IDENTIFIER LITERAL_NUM

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "tree_handler.h"
%}

%union
{
    double varValue;
    char *stringValue;
    void *nodeValue;
}

%type<stringValue> VAR IDENTIFIER
%type<varValue> LITERAL_NUM
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
	: { $$ = handle_arg_def_block(); } { $$ = handle_statement_block(); }
	VAR IDENTIFIER '(' argument_defintion_block ')' '{' statement_block '}' 
	{
		handle_function_def($<nodeValue>1, $<nodeValue>2);
	}
	;

function_call
	: { $$ = handle_arg_block(); }
	IDENTIFIER '(' argument_block ')' 
	{ 
		handle_func_call(get_pointer_symbol($2), $<nodeValue>1);
	}
	;

argument_defintion_block
	: /* empty */
	| argument_defintion_block ',' argument_defintion
	| argument_defintion
	;

argument_defintion
	: VAR IDENTIFIER 
	{
		 handle_arg_def(TYPE_VAR, $2, 0);
	}
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
	: IDENTIFIER '=' IDENTIFIER { printf("Yacc found assignment with operands %s and %s\n", $1, $3); }
	| IDENTIFIER '=' LITERAL_NUM { printf("Yacc found assignment with operands %s and %f\n", $1, $3); }
	;

argument_block
	:	/* empty */
	| argument_block ',' argument
	| argument
	;

argument
	: IDENTIFIER 
	{
		node_t *arg = create_node(current_parent_node, NODE_ARG);
		((arg_data*) arg->data)->identifier = get_pointer_symbol($1);
		
		node_t* child[1] = { arg };
		add_child_to_parent_block(current_parent_node, 1, child);
	}
	| LITERAL_NUM 
	{
		node_t *arg = create_node(current_parent_node, NODE_ARG);
		((arg_data*) arg->data)->identifier = get_literal_num_symbol($1);
		
		node_t* child[1] = { arg };
		add_child_to_parent_block(current_parent_node, 1, child);
	}
	;
%%

void yyerror (char *s) 
{
   fprintf (stderr, "%s\n", s);
}