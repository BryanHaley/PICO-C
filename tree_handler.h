#ifndef TREE_HANDLER_H
#define TREE_HANDLER_H

#include <stdbool.h>
#include "ast.h"

tree_t *syntax_tree;
node_t* current_arg_def_block;
node_t* current_statement_block;
node_t* current_arg_block;

node_t* create_node(node_type_e node_type);
parent_block_data* add_child_to_parent_block(node_t* parent, 
                                             int num_children, node_t** children);

void create_global_block();
void add_func_def_to_global_block(node_t* node);

node_t* create_func_def_node(char* return_type, char* identifier, node_t* arg_def_block, node_t* stmnt_block);
node_t* create_func_call_node(char* identifier, node_t* arg_block);
node_t* create_arg_def_node(char* type, char* identifier);
node_t* create_assign_node(char* identifier, node_t* expr);
node_t* create_declaration_node(char* type, char* identifier);
node_t* create_declaration_with_assign_node(char* type, char* identifier, node_t* expr);

node_t* create_bin_expr_node(node_t* left_node, node_t* right_node, char op);
node_t* create_primary_node_num(primary_type_e val_type, double val);
node_t* create_primary_node_str(primary_type_e val_type, char* val);
node_t* create_primary_node_nde(primary_type_e val_type, node_t* val);

void handle_arg_node(node_t* node);
void handle_stmnt_node(node_t* node);
void handle_null_stmnt_node();

void clear_arg_def_block();
void clear_statement_block();
void clear_arg_block();

#endif
