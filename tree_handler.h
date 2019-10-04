#ifndef TREE_HANDLER_H
#define TREE_HANDLER_H

#include <stdbool.h>
#include "ast.h"

tree_t *syntax_tree;

node_t* create_node(node_type_e node_type);
parent_block_data* add_child_to_parent_block(node_t* parent, node_t* child);
parent_block_data* add_children_to_parent_block(node_t* parent, 
                                                int num_children, node_t** children);

void create_global_block();
void add_func_def_to_global_block(node_t* node);

node_t* create_func_def_node(char* return_type, char* identifier, node_t* arg_def_block, node_t* stmnt_block);
node_t* create_func_call_node(char* identifier, node_t* arg_block);
node_t* create_arg_def_node(char* type, char* identifier);
node_t* create_assign_node(node_t* dest, node_t* expr, char* op);
node_t* create_declaration_node(char* type, char* identifier);
node_t* create_declaration_with_assign_node(char* type, char* identifier, node_t* expr);
node_t* create_array_access_node(char* identifier, node_t* accessor);
node_t* create_array_accessor_node(node_t* expr);
node_t* create_array_dec_node(char* identifier, int size, node_t* literal_block);
node_t* create_array_dim_node(double num);
node_t* create_multi_dim_array_dec_node(char* identifier, node_t* dimensions, node_t* literal_block);
node_t* create_struct_def_node(char* identifier, node_t* member_block);
node_t* create_struct_init_node(char* var_type, char* identifier, char* obj_type);
node_t* create_symbol_node(char* identifier);

node_t* create_bin_expr_node(node_t* left_node, node_t* right_node, char* op);
node_t* create_primary_node(primary_type_e val_type, void* val);
node_t* create_postfix_node(char* identifier, char* op);

node_t* handle_parent_block(node_t* parent, node_type_e parent_type, node_t* child);
node_t* set_expr_paren(node_t* node);
node_t* set_expr_unary(node_t* node, char* unary);

#endif
