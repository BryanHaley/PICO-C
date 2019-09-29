%token VAR STRING BOOL ARRAY IDENTIFIER LITERAL_NUM LITERAL_STRING LITERAL_BOOL
%token PLUS_EQUAL MINUS_EQUAL TIMES_EQUAL DIVIDE_EQUAL MODULO_EQUAL
%token RIGHT_SHIFT_EQUAL LEFT_SHIFT_EQUAL AND_EQUAL OR_EQUAL XOR_EQUAL
%token RIGHT_SHIFT LEFT_SHIFT PLUS_PLUS MINUS_MINUS LOGICAL_AND LOGICAL_OR
%token LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL EQUAL_EQUAL NOT_EQUAL
%token GLOBAL LENGTH UNARY

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

%type<stringValue> IDENTIFIER LITERAL_STRING type unary_operator
%type<varValue> LITERAL_NUM
%type<boolValue> LITERAL_BOOL
%type<nodeValue> function_def function_call argument_defintion assignment
%type<nodeValue> expression primary declaration declaration_with_assign statement argument
%type<nodeValue> argument_definition_block statement_block argument_block postfix
%type<nodeValue> array_declaration array_accessor literal literal_block

%left PLUS_PLUS MINUS_MINUS '(' ')' '[' ']' '{' '}'
%left '*' '/' '%'
%left '+' '-'
%left LEFT_SHIFT RIGHT_SHIFT
%left '<' LESS_THAN_OR_EQUAL
%left '>' GREATER_THAN_OR_EQUAL
%left EQUAL_EQUAL NOT_EQUAL
%left '&'
%left '^'
%left '|'
%left LOGICAL_AND
%left LOGICAL_OR
%right '?' ':'
%right '=' PLUS_EQUAL MINUS_EQUAL TIMES_EQUAL DIVIDE_EQUAL MODULO_EQUAL
%right RIGHT_SHIFT_EQUAL LEFT_SHIFT_EQUAL AND_EQUAL OR_EQUAL XOR_EQUAL
%right UNARY
%left ','

%%
program
    : { create_global_block(); } global_block 
    ;

global_block
    : function_def
    { 
        $1->global = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | declaration ';'
    {
        $1->global = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | declaration_with_assign ';'
    {
        $1->global = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | array_declaration ';'
    {
        $1->global = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    ;

function_def
    : type IDENTIFIER '(' ')' '{' statement_block '}' 
    {
        $6->increase_indent = true; 
        $$ = create_func_def_node($1, $2, NULL, $6);
    }
    | type IDENTIFIER '(' argument_definition_block ')' '{' statement_block '}' 
    {
        $7->increase_indent = true; 
        $$ = create_func_def_node($1, $2, $4, $7);
    }
    ;

argument_definition_block
    : argument_defintion
    {
        node_t* arg_def_block = create_node(NODE_ARG_DEF_BLOCK);
        add_child_to_parent_block(arg_def_block, $1);
        $$ = arg_def_block;
    }
    | argument_definition_block ',' argument_defintion
    { $$ = handle_parent_block($1, NODE_ARG_DEF_BLOCK, $3); }
    ;

argument_defintion
    : type IDENTIFIER ',' 
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
    {
        $1->end_line = true;
        $$ = $1;
    }
    | declaration_with_assign ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | declaration ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | assignment ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | postfix ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | array_declaration ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | ';'
    { $$ = NULL; }
    ;

postfix
    : IDENTIFIER PLUS_PLUS
    { $$ = create_postfix_node($1, "++"); }
    | IDENTIFIER MINUS_MINUS
    { $$ = create_postfix_node($1, "--"); }
    ;

assignment
    : IDENTIFIER '=' expression
    { $$ = create_assign_node($1, $3, "="); }
    | IDENTIFIER PLUS_EQUAL expression
    { $$ = create_assign_node($1, $3, "+="); }
    | IDENTIFIER MINUS_EQUAL expression
    { $$ = create_assign_node($1, $3, "-="); }
    | IDENTIFIER TIMES_EQUAL expression
    { $$ = create_assign_node($1, $3, "*="); }
    | IDENTIFIER DIVIDE_EQUAL expression
    { $$ = create_assign_node($1, $3, "/="); }
    | IDENTIFIER MODULO_EQUAL expression
    { $$ = create_assign_node($1, $3, "%="); }
    | IDENTIFIER RIGHT_SHIFT_EQUAL expression
    { $$ = create_assign_node($1, $3, ">>="); }
    | IDENTIFIER LEFT_SHIFT_EQUAL expression
    { $$ = create_assign_node($1, $3, "<<="); }
    | IDENTIFIER AND_EQUAL expression
    { $$ = create_assign_node($1, $3, "&="); }
    | IDENTIFIER OR_EQUAL expression
    { $$ = create_assign_node($1, $3, "|="); }
    | IDENTIFIER XOR_EQUAL expression
    { $$ = create_assign_node($1, $3, "^="); }
    ;

function_call
    : IDENTIFIER '(' argument_block ')' 
    {
        node_t* node = create_func_call_node($1, $3);
        node->end_line = true;
        $$ = node;
    }
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

array_declaration
    : ARRAY IDENTIFIER '[' ']' '=' '{' '}'
    { $$ = create_array_dec_node($2, 0, NULL); }
    | ARRAY IDENTIFIER '[' ']' '=' '{' literal_block '}'
    { $$ = create_array_dec_node($2, 0, $7); }
    | ARRAY IDENTIFIER '[' LITERAL_NUM ']' '=' '{' literal_block '}'
    { $$ = create_array_dec_node($2, (int) $4, $8); }
    | ARRAY IDENTIFIER '[' ']'
    { $$ = create_array_dec_node($2, 0, NULL); }
    | GLOBAL ARRAY IDENTIFIER '[' ']' '=' '{' '}'
    { 
        node_t* node = create_array_dec_node($3, 0, NULL);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY IDENTIFIER '[' ']' '=' '{' literal_block '}'
    {
        node_t* node = create_array_dec_node($3, 0, $8);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY IDENTIFIER '[' LITERAL_NUM ']' '=' '{' literal_block '}'
    {
        node_t* node = create_array_dec_node($3, (int) $5, $9);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY IDENTIFIER '[' ']'
    {
        node_t* node = create_array_dec_node($3, 0, NULL);
        node->global = true;
        $$ = node;
    }
    ;

array_accessor
    : IDENTIFIER '[' expression ']'
    { $$ = create_array_accessor_node($1, $3); }
    ;

declaration
    : type IDENTIFIER
    { $$ = create_declaration_node($1, $2); }
    | GLOBAL type IDENTIFIER
    {
        node_t* node = create_declaration_node($2, $3);
        node->global = true;
        $$ = node;
    }
    ;

declaration_with_assign
    : type IDENTIFIER '=' expression
    { $$ = create_declaration_with_assign_node($1, $2, $4); }
    | GLOBAL type IDENTIFIER '=' expression
    {
        node_t* node = create_declaration_with_assign_node($2, $3, $5);
        node->global = true;
        $$ = node;
    }
    ;

expression
    : expression '+' expression
    { $$ = create_bin_expr_node($1, $3, "+"); }
    | expression '-' expression
    { $$ = create_bin_expr_node($1, $3, "-"); }
    | expression '*' expression
    { $$ = create_bin_expr_node($1, $3, "*"); }
    | expression '/' expression
    { $$ = create_bin_expr_node($1, $3, "/"); }
    | expression '%' expression
    { $$ = create_bin_expr_node($1, $3, "%"); }
    | expression RIGHT_SHIFT expression
    { $$ = create_bin_expr_node($1, $3, ">>"); }
    | expression LEFT_SHIFT expression
    { $$ = create_bin_expr_node($1, $3, "<<"); }
    | expression '&' expression
    { $$ = create_bin_expr_node($1, $3, "&"); }
    | expression '|' expression
    { $$ = create_bin_expr_node($1, $3, "|"); }
    | expression '^' expression
    { $$ = create_bin_expr_node($1, $3, "^"); }
    | '(' expression ')'
    { 
        $$ = set_expr_paren($2);
    }
    | unary_operator expression %prec UNARY
    {
        $$ = set_expr_unary($2, $1);
    }
    | primary
    { $$ = $1; }
    ;

primary
    : literal
    { $$ = $1; }
    | IDENTIFIER
    { $$ = create_primary_node_str(PRI_IDENTIFIER, $1); }
    | function_call
    { $$ = create_primary_node_nde(PRI_FUNC_CALL, $1); }
    | array_accessor
    { $$ = create_primary_node_nde(PRI_ARR_ACCESS, $1); }
    ;

literal
    : LITERAL_NUM
    { $$ = create_primary_node_num(PRI_LITERAL_NUM, $1); }
    | LITERAL_STRING
    { $$ = create_primary_node_str(PRI_LITERAL_STR, $1); }
    ;

literal_block
    : literal
    {
        node_t* literal_block = create_node(NODE_LIT_BLOCK);
        add_child_to_parent_block(literal_block, $1);
        $$ = literal_block;
    }
    | literal_block ',' literal
    { $$ = handle_parent_block($1, NODE_LIT_BLOCK, $3); }
    ;

type
    : VAR { $$ = "var"; }
    | BOOL { $$ = "bool"; }
    | STRING { $$ = "string"; }
    | ARRAY { $$ = "array"; }
    | IDENTIFIER { $$ = $1; }
    ;

unary_operator
    : '-'
    { $$ = "-"; }
    | LENGTH
    { $$ = "length"; }
    | '!'
    { $$ = "not"; }
    ;
%%

void yyerror (char *s) 
{
   fprintf (stderr, "%s\n", s);

   err_in_parse = true;
}
