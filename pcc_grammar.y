%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pcc_grammar.h"
#include "ast.h"
#include "tree_handler.h"
#include "tree_checker.h"
#include "code_gen.h"

FILE *yyin;
%}

%glr-parser
%locations

%union
{
    double  varValue;
    char*   stringValue;
    bool    boolValue;
    node_t* nodeValue;
}

%token VAR STRING BOOL ARRAY IDENTIFIER LITERAL_NUM LITERAL_STRING LITERAL_BOOL DEFAULT
%token PLUS_EQUAL MINUS_EQUAL TIMES_EQUAL DIVIDE_EQUAL MODULO_EQUAL DO WHILE SWITCH 
%token RIGHT_SHIFT_EQUAL LEFT_SHIFT_EQUAL AND_EQUAL OR_EQUAL XOR_EQUAL GOTO CONTINUE
%token RIGHT_SHIFT LEFT_SHIFT PLUS_PLUS MINUS_MINUS LOGICAL_AND LOGICAL_OR RETURN CONSTRUCTOR
%token LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL EQUAL_EQUAL NOT_EQUAL FOR BREAK CASE
%token STRUCT GLOBAL LENGTH UNARY NEW IF ELSEIF ELSE UNTIL FASTSWITCH TK_NULL FUNC

%type<stringValue> IDENTIFIER LITERAL_STRING unary_operator
%type<varValue> LITERAL_NUM
%type<boolValue> LITERAL_BOOL
%type<nodeValue> function_def function_call argument_defintion assignment break_statement
%type<nodeValue> expression declaration declaration_with_assign statement argument
%type<nodeValue> argument_definition_block statement_block argument_block postfix
%type<nodeValue> array_declaration literal literal_block struct_init struct_definition
%type<nodeValue> struct_member_definition_block struct_member_definition array_dimension
%type<nodeValue> array_access array_accessor array_multi_access array_dimension_block
%type<nodeValue> multi_dim_array_declaration assignment_dest object_access do_until_loop
%type<nodeValue> relational_expression method_call statement_opt_braces goto_statement
%type<nodeValue> if_statement elseif_statement_block elseif_statement else_statement
%type<nodeValue> for_loop for_assign for_inc do_while_loop while_loop label_maker
%type<nodeValue> return_statement continue_statement switch_statement case_block case
%type<nodeValue> fast_switch_statement fswitch_call struct_constructor_def symbol type

%nonassoc NO_ELSE
%nonassoc ELSEIF_NO_ELSE
%nonassoc ELSEIF_ELSE
%nonassoc IF_ELSE

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
    : global_statement
    | global_block global_statement
    ;

global_statement
    : function_def
    { 
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | struct_definition
    {
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | declaration ';'
    {
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | declaration_with_assign ';'
    {
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | array_declaration ';'
    {
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | multi_dim_array_declaration ';'
    {
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    | fast_switch_statement
    {
        $1->global = true; $1->global_statement = true; $1->end_line = true;
        add_child_to_parent_block(syntax_tree->global_block, $1);
    }
    ;

if_statement
    : IF '(' relational_expression ')' statement_opt_braces %prec NO_ELSE
    { $$ = create_if_statement_node(yylineno, $3, $5, NULL, NULL); }
    | IF '(' relational_expression ')' statement_opt_braces else_statement %prec IF_ELSE
    { $$ = create_if_statement_node(yylineno, $3, $5, NULL, $6); }
    | IF '(' relational_expression ')' statement_opt_braces elseif_statement_block %prec ELSEIF_NO_ELSE
    { $$ = create_if_statement_node(yylineno, $3, $5, $6, NULL); }
    | IF '(' relational_expression ')' statement_opt_braces elseif_statement_block else_statement %prec ELSEIF_ELSE
    { $$ = create_if_statement_node(yylineno, $3, $5, $6, $7); }
    ;

elseif_statement_block
    : elseif_statement
    {
        node_t* elseif_statement_block = create_node(NODE_ELSEIF_BLOCK, yylineno);
        add_child_to_parent_block(elseif_statement_block, $1);
        $$ = elseif_statement_block;
    }
    | elseif_statement_block elseif_statement
    { $$ = handle_parent_block(yylineno, $1, NODE_ELSEIF_BLOCK, $2); }
    ;

elseif_statement
    : ELSEIF '(' relational_expression ')' statement_opt_braces
    { $$ = create_elseif_statement_node(yylineno, $3, $5); }
    ;

else_statement
    : ELSE statement_opt_braces
    { $$ = create_else_statement_node(yylineno, $2); }
    ;

while_loop
    : WHILE '(' relational_expression ')' statement_opt_braces
    { $$ = create_while_loop_node(yylineno, $3, $5); }
    ;

do_while_loop
    : DO '{' statement_block '}' WHILE '(' relational_expression ')'
    {
        $3->increase_indent = true;
        $$ = create_do_while_loop_node(yylineno, $3, $7);
    }
    ;

do_until_loop
    : DO '{' statement_block '}' UNTIL '(' relational_expression ')'
    {
        $3->increase_indent = true;
        $$ = create_do_until_loop_node(yylineno, $3, $7);
    }
    ;

for_loop
    : FOR '(' for_assign ';' relational_expression ';' for_inc ')' statement_opt_braces
    { $$ = create_for_loop_node(yylineno, $3, $5, $7, $9); }
    ;

for_assign
    : declaration_with_assign
    { $$ = $1; }
    | assignment
    { $$ = $1; }
    ;

for_inc
    : assignment
    { $$ = $1; }
    | postfix
    { $$ = $1; }
    ;

/* statement(s) with optional braces */
statement_opt_braces
    : '{' statement_block '}'
    {
        $2->increase_indent = true;
        $$ = $2;
    }
    | statement
    {
        $1->increase_indent = true;
        $$ = $1;
    }
    ;

struct_definition
    : STRUCT symbol '{' struct_member_definition_block '}'
    {
        $4->increase_indent = true;
        $$ = create_struct_def_node(yylineno, $2, $4);
    }
    ;

struct_member_definition_block
    : struct_member_definition
    {
        node_t* struct_member_definition_block = create_node(NODE_STRUCT_MEM_BLOCK, yylineno);
        add_child_to_parent_block(struct_member_definition_block, $1);
        $$ = struct_member_definition_block;
    }
    | struct_member_definition_block struct_member_definition
    { $$ = handle_parent_block(yylineno, $1, NODE_STRUCT_MEM_BLOCK, $2); }
    ;

struct_member_definition
    : declaration_with_assign ';'
    {
        $1->member = true;
        $$ = $1;
    }
    | declaration ';'
    {
        $1->member = true;
        $$ = $1;
    }
    | struct_constructor_def
    {
        $1->member = true;
        $$ = $1;
    }
    ;

struct_init
    : NEW type
    { $$ = create_struct_init_node(yylineno, $2, NULL); }
    | NEW function_call
    { $$ = create_struct_init_node(yylineno, NULL, $2); }
    ;

function_def
    : type symbol '(' ')' '{' statement_block '}' 
    {
        $6->increase_indent = true; 
        $$ = create_func_def_node(yylineno, $1, $2, NULL, $6);
    }
    | type symbol '(' argument_definition_block ')' '{' statement_block '}' 
    {
        $7->increase_indent = true; 
        $$ = create_func_def_node(yylineno, $1, $2, $4, $7);
    }
    ;

struct_constructor_def
    : CONSTRUCTOR '(' ')' '{' statement_block '}' 
    {
        $5->increase_indent = true; 
        $$ = create_struct_constructor_node(yylineno, NULL, $5);
    }
    | CONSTRUCTOR '(' argument_definition_block ')' '{' statement_block '}' 
    {
        $6->increase_indent = true; 
        $$ = create_struct_constructor_node(yylineno, $3, $6);
    }
    ;

argument_definition_block
    : argument_defintion
    {
        node_t* arg_def_block = create_node(NODE_ARG_DEF_BLOCK, yylineno);
        add_child_to_parent_block(arg_def_block, $1);
        $$ = arg_def_block;
    }
    | argument_definition_block ',' argument_defintion
    { $$ = handle_parent_block(yylineno, $1, NODE_ARG_DEF_BLOCK, $3); }
    ;

argument_defintion
    : type symbol
    { $$ = create_arg_def_node(yylineno, $1, $2); }
    ;

statement_block
    : statement_block statement
    { $$ = handle_parent_block(yylineno, $1, NODE_STMNT_BLOCK, $2); }
    | /* empty */
    { $$ = NULL; }
    ;

statement
    : function_call ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | method_call ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | assignment ';'
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
    | multi_dim_array_declaration ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | struct_init ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | if_statement
    {
        $1->end_line = true;
        $$ = $1;
    }
    | for_loop
    {
        $1->end_line = true;
        $$ = $1;
    }
    | do_while_loop ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | do_until_loop ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | while_loop
    {
        $1->end_line = true;
        $$ = $1;
    }
    | label_maker
    {
        $1->end_line = true;
        $$ = $1;
    }
    | goto_statement ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | break_statement ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | continue_statement ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | return_statement ';'
    {
        $1->end_line = true;
        $$ = $1;
    }
    | switch_statement
    {
        $1->end_line = true;
        $$ = $1;
    }
    | fast_switch_statement
    {
        $1->end_line = true;
        $$ = $1;
    }
    | fswitch_call
    {
        $1->end_line = true;
        $$ = $1;
    }
    | ';'
    { $$ = NULL; }
    ;

label_maker
    : symbol ':'
    { $$ = create_labelmaker_node(yylineno, $1); }
    ;

goto_statement
    : GOTO symbol
    { $$ = create_goto_statement_node(yylineno, $2); }
    ;

break_statement
    : BREAK
    { $$ = create_break_statement_node(yylineno); }
    ;

continue_statement
    : CONTINUE
    { $$ = create_continue_statement_node(yylineno); }
    ;

return_statement
    : RETURN expression
    { $$ = create_return_statement_node(yylineno, $2); }
    | RETURN
    { $$ = create_return_statement_node(yylineno, NULL); }
    ;

switch_statement
    : SWITCH '(' expression ')' '{' case_block '}'
    {
        $6->increase_indent = true;
        $$ = create_switch_statement_node(yylineno, $3, $6);
    }
    ;

fast_switch_statement
    : FASTSWITCH type symbol '(' argument_definition_block ')' '{' case_block '}'
    {
        $8->increase_indent = true;
        $$ = create_fast_switch_statement_node(yylineno, $2, $3, $5, $8);
    }
    | FASTSWITCH type symbol '(' ')' '{' case_block '}'
    {
        $7->increase_indent = true;
        $$ = create_fast_switch_statement_node(yylineno, $2, $3, NULL, $7);
    }
    ;

case_block
    : case
    {
        node_t* case_block = create_node(NODE_CASE_BLOCK, yylineno);
        add_child_to_parent_block(case_block, $1);
        $$ = case_block;
    }
    | case_block case
    { $$ = handle_parent_block(yylineno, $1, NODE_CASE_BLOCK, $2); }
    ;

case
    : CASE '(' expression ')' ':' statement_block
    {
        $6->increase_indent = true;
        $$ = create_case_node(yylineno, $3, $6);
    }
    | DEFAULT ':' statement_block
    {
        $3->increase_indent = true;
        $$ = create_case_node(yylineno, NULL, $3);
    }
    ;

postfix
    : symbol PLUS_PLUS
    { $$ = create_postfix_node(yylineno, $1, "++"); }
    | symbol MINUS_MINUS
    { $$ = create_postfix_node(yylineno, $1, "--"); }
    ;

assignment
    : assignment_dest '=' expression
    { $$ = create_assign_node(yylineno, $1, $3, "="); }
    | assignment_dest PLUS_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "+="); }
    | assignment_dest MINUS_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "-="); }
    | assignment_dest TIMES_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "*="); }
    | assignment_dest DIVIDE_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "/="); }
    | assignment_dest MODULO_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "%="); }
    | assignment_dest RIGHT_SHIFT_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, ">>="); }
    | assignment_dest LEFT_SHIFT_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "<<="); }
    | assignment_dest AND_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "&="); }
    | assignment_dest OR_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "|="); }
    | assignment_dest XOR_EQUAL expression
    { $$ = create_assign_node(yylineno, $1, $3, "^="); }
    ;

assignment_dest
    : array_access
    { $$ = $1; }
    | object_access
    { $$ = $1; }
    | symbol
    { $$ = $1; }
    ;

object_access
    : symbol '.'
    {
        node_t* object_access_block = create_node(NODE_OBJ_ACCESSOR_BLOCK, yylineno);
        add_child_to_parent_block(object_access_block, $1);
        $$ = object_access_block;
    }
    | object_access symbol
    {
        $$ = handle_parent_block(yylineno, $1, NODE_OBJ_ACCESSOR_BLOCK, $2);
    }
    ;

method_call
    : object_access function_call
    { $$ = handle_parent_block(yylineno, $1, NODE_OBJ_ACCESSOR_BLOCK, $2); }
    ;

function_call
    : symbol '(' argument_block ')' 
    { $$ = create_func_call_node(yylineno, $1, $3); }
    | symbol '(' ')' 
    { $$ = create_func_call_node(yylineno, $1, NULL); }
    ;

fswitch_call
    : symbol '[' expression ']' '(' argument_block ')' 
    { $$ = create_fswitch_call_node(yylineno, $1, $3, $6); }
    | symbol '[' expression ']' '(' ')' 
    { $$ = create_fswitch_call_node(yylineno, $1, $3, NULL); }
    ;

argument_block
    : argument
    {
        node_t* arg_block = create_node(NODE_ARG_BLOCK, yylineno);
        add_child_to_parent_block(arg_block, $1);
        $$ = arg_block;
    }
    | argument_block ',' argument
    { $$ = handle_parent_block(yylineno, $1, NODE_ARG_BLOCK, $3); }
    ;

argument
    : expression ','
    { $$ = $1; }
    | expression
    { $$ = $1; }
    ;

array_declaration
    : ARRAY symbol '[' ']' '=' '{' '}'
    { $$ = create_array_dec_node(yylineno, $2, 0, NULL); }
    | ARRAY symbol '[' ']' '=' '{' literal_block '}'
    { $$ = create_array_dec_node(yylineno, $2, 0, $7); }
    | ARRAY symbol '[' LITERAL_NUM ']' '=' '{' literal_block '}'
    { $$ = create_array_dec_node(yylineno, $2, (int) $4, $8); }
    | ARRAY symbol '[' ']'
    { $$ = create_array_dec_node(yylineno, $2, 0, NULL); }
    | GLOBAL ARRAY symbol '[' ']' '=' '{' '}'
    { 
        node_t* node = create_array_dec_node(yylineno, $3, 0, NULL);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY symbol '[' ']' '=' '{' literal_block '}'
    {
        node_t* node = create_array_dec_node(yylineno, $3, 0, $8);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY symbol '[' LITERAL_NUM ']' '=' '{' literal_block '}'
    {
        node_t* node = create_array_dec_node(yylineno, $3, (int) $5, $9);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY symbol '[' ']'
    {
        node_t* node = create_array_dec_node(yylineno, $3, 0, NULL);
        node->global = true;
        $$ = node;
    }
    ;

multi_dim_array_declaration
    : ARRAY symbol array_dimension_block '=' '{' '}'
    { $$ = create_multi_dim_array_dec_node(yylineno, $2, $3, NULL); }
    | ARRAY symbol array_dimension_block '=' '{' literal_block '}'
    { $$ = create_multi_dim_array_dec_node(yylineno, $2, $3, $6); }
    | GLOBAL ARRAY symbol array_dimension_block '=' '{' '}'
    {
        node_t* node = create_multi_dim_array_dec_node(yylineno, $3, $4, NULL);
        node->global = true;
        $$ = node;
    }
    | GLOBAL ARRAY symbol array_dimension_block '=' '{' literal_block '}'
    {
        node_t* node = create_multi_dim_array_dec_node(yylineno, $3, $4, $7);
        node->global = true;
        $$ = node;
    }
    | ARRAY symbol array_dimension_block
    { $$ = create_multi_dim_array_dec_node(yylineno, $2, $3, NULL); }
    ;

array_dimension_block
    : array_dimension
    {
        node_t* array_dimension_block = create_node(NODE_ARR_DIM_BLOCK, yylineno);
        add_child_to_parent_block(array_dimension_block, $1);
        $$ = array_dimension_block;
    }
    | array_dimension_block array_dimension
    { $$ = handle_parent_block(yylineno, $1, NODE_ARR_DIM_BLOCK, $2); }
    ;

array_dimension
    : '[' LITERAL_NUM ']'
    { $$ = create_array_dim_node(yylineno, $2); }
    ;

array_access
    : symbol array_multi_access
    { $$ = create_array_access_node(yylineno, $1, $2); }
    ;

array_multi_access
    : array_accessor
    {
        node_t* arr_m_accessor = create_node(NODE_ARR_MULTI_ACCESSOR, yylineno);
        add_child_to_parent_block(arr_m_accessor, $1);
        $$ = arr_m_accessor;
    }
    | array_multi_access array_accessor
    { $$ = handle_parent_block(yylineno, $1, NODE_ARR_MULTI_ACCESSOR, $2); }
    ;

array_accessor
    : '[' expression ']'
    { $$ = create_array_accessor_node(yylineno, $2); }
    ;

declaration
    : type symbol
    { $$ = create_declaration_node(yylineno, $1, $2); }
    | GLOBAL type symbol
    {
        node_t* node = create_declaration_node(yylineno, $2, $3);
        node->global = true;
        $$ = node;
    }
    ;

declaration_with_assign
    : type symbol '=' expression
    { $$ = create_declaration_with_assign_node(yylineno, $1, $2, $4); }
    | GLOBAL type symbol '=' expression
    {
        node_t* node = create_declaration_with_assign_node(yylineno, $2, $3, $5);
        node->global = true;
        $$ = node;
    }
    ;

expression
    : expression '+' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "+"); }
    | expression '-' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "-"); }
    | expression '*' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "*"); }
    | expression '/' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "/"); }
    | expression '%' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "%"); }
    | expression RIGHT_SHIFT expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, ">>"); }
    | expression LEFT_SHIFT expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "<<"); }
    | expression '&' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "&"); }
    | expression '|' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "|"); }
    | expression '^' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "^"); }
    | '(' expression ')'
    { 
        $$ = set_expr_paren(yylineno, $2);
    }
    | unary_operator expression %prec UNARY
    {
        $$ = set_expr_unary(yylineno, $2, $1);
    }
    | relational_expression
    { $$ = $1; }
    | method_call
    { $$ = $1; }
    | function_call
    { $$ = $1; }
    | fswitch_call
    { $$ = $1; }
    | array_access
    { $$ = $1; }
    | object_access
    { $$ = $1; }
    | struct_init
    { $$ = $1; }
    | symbol
    { $$ = $1; }
    | LITERAL_NUM
    { $$ = create_primary_node(yylineno, PRI_LITERAL_NUM, (void*) &$1); }
    | LITERAL_STRING
    { $$ = create_primary_node(yylineno, PRI_LITERAL_STR, (void*) $1); }
    | TK_NULL
    { $$ = create_null_node(yylineno); }
    ;

relational_expression
    : expression LOGICAL_AND expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "and"); }
    | expression LOGICAL_OR expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "or"); }
    | expression LESS_THAN_OR_EQUAL expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "<="); }
    | expression GREATER_THAN_OR_EQUAL expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, ">="); }
    | expression '<' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "<"); }
    | expression '>' expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, ">"); }
    | expression EQUAL_EQUAL expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "=="); }
    | expression NOT_EQUAL expression
    { $$ = create_bin_expr_node(yylineno, $1, $3, "~="); }
    | LITERAL_BOOL
    { $$ = create_primary_node(yylineno, PRI_LITERAL_BOOL, (void*) &$1); }
    ;

literal
    : LITERAL_NUM
    { $$ = create_primary_node(yylineno, PRI_LITERAL_NUM, (void*) &$1); }
    | LITERAL_STRING
    { $$ = create_primary_node(yylineno, PRI_LITERAL_STR, (void*) $1); }
    | LITERAL_BOOL
    { $$ = create_primary_node(yylineno, PRI_LITERAL_BOOL, (void*) &$1); }
    ;

literal_block
    : literal
    {
        node_t* literal_block = create_node(NODE_LIT_BLOCK, yylineno);
        add_child_to_parent_block(literal_block, $1);
        $$ = literal_block;
    }
    | literal_block ',' literal
    { $$ = handle_parent_block(yylineno, $1, NODE_LIT_BLOCK, $3); }
    ;

type
    : VAR
    { $$ = create_type_symbol_node(yylineno, "var"); }
    | BOOL
    { $$ = create_type_symbol_node(yylineno, "bool"); }
    | STRING
    { $$ = create_type_symbol_node(yylineno, "string"); }
    | ARRAY
    { $$ = create_type_symbol_node(yylineno, "array"); }
    | STRUCT
    { $$ = create_type_symbol_node(yylineno, "struct"); }
    | FUNC
    { $$ = create_type_symbol_node(yylineno, "func"); }
    | symbol
    { $$ = $1; }
    ;

symbol
    : IDENTIFIER
    { $$ = create_symbol_node(yylineno, $1); }
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

void yyerror (const char *s) 
{
   fprintf(stderr, "Error parsing line: %d\n", yylineno);

   err_in_parse = true;
}

int main(int argc, char** argv) 
{    
    yyin = fopen(argv[1], "r");
    yyparse();

    check_tree(syntax_tree);

    if (!err_in_lex && !err_in_parse && !err_in_tree && !err_in_code_gen)
    {
        FILE* out = fopen(argv[2], "w+");
        
        generate_code(out, syntax_tree);

        fclose(out);
    }

    else if (err_in_lex)
    {
        fprintf(stderr, "Error while parsing lexical tokens. Aborting code generation.\n");
    }

    else if (err_in_parse)
    {
        fprintf(stderr, "Error while parsing grammar. Aborting code generation.\n");
    }

    else if (err_in_tree)
    {
        fprintf(stderr, "Error in building abstract syntax tree. Aborting code generation.\n");
    }

    else if (err_in_code_gen)
    {
        fprintf(stderr, "Error in code generation. Aborting.\n");
    }

    else
    {
        fprintf(stderr, "Unknown error. Aborting.\n");
        fclose(yyin);
        return 0; 
    }

    fclose(yyin);
    return 0; 
}