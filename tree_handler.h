#ifndef TREE_HANDLER_H
#define TREE_HANDLER_H

#include <stdbool.h>
#include "ast.h"

tree_t *syntax_tree;
node_t* current_parent_node;
node_t* current_scope;

node_t* create_node(node_t *parent, node_type_e node_type);
parent_block_data* add_child_to_parent_block(node_t* parent, 
                                             int num_children, node_t** children);

symbol_t* get_symbol_by_identifier(node_t* scope, char* identifier);
symbol_t* get_var_symbol(node_t* scope, char* name, double value, 
                         bool create_if_DNE, bool fail_if_exists);
symbol_t* get_pointer_symbol(node_t* scope, char* name, 
                             bool create_if_DNE, bool fail_if_exists);
symbol_t* get_literal_num_symbol(double value);
symbol_t* get_literal_str_symbol(char* strVal);
symbol_t* get_literal_bool_symbol(bool boolVal);

// nodes that don't set themselves as parents
void handle_function_def(node_t* arg_def_block, node_t* statement_block);
void handle_func_call(symbol_t* func, node_t* arg_block);
void handle_arg_def(data_types_e type, char* symbol_name);
void handle_assignment_to_identifier(char* iden_name_left, char *iden_name_right);
void handle_assignment_to_literal(char* iden_name, data_types_e type, symbol_value val);
//void handle_assignment_to_expr(char* iden_name, node_t* expr);

// nodes that set themselves as parents
void handle_global_block(); // returns void because it's the top level node
node_t* handle_arg_def_block();
node_t* handle_statement_block();
node_t* handle_arg_block();

#endif
