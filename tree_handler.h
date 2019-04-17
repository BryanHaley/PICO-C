#ifndef TREE_HANDLER_H
#define TREE_HANDLER_H

#include "ast.h"

int yylex(void);
void yyerror(char *s);

tree_t *syntax_tree;
node_t* current_parent_node;
node_t* current_scope;

node_t* create_node(node_t *parent, node_type_e node_type);
parent_block_data* add_child_to_parent_block(node_t* parent, int num_children, node_t** children);
symbol_t* get_var_symbol(char* name, double value);
symbol_t* get_pointer_symbol(char* name);
symbol_t* get_literal_num_symbol(double value);

void handle_global_block();
void handle_function_def(node_t* arg_def_block, node_t* statement_block);
void handle_func_call(symbol_t* func, node_t* arg_block);
void handle_arg_def(data_types_e type, char* symbol_name, double symbol_value);

node_t* handle_arg_def_block();
node_t* handle_statement_block();
node_t* handle_arg_block();

#endif