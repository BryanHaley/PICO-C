#ifndef TREE_HANDLER_H
#define TREE_HANDLER_H

#include <stdbool.h>
#include "ast.h"

tree_t *syntax_tree;

node_t* create_node(node_type_e node_type, int line_no);
parent_block_data* add_child_to_parent_block(node_t* parent, node_t* child);
parent_block_data* add_children_to_parent_block(node_t* parent, 
                                                int num_children, node_t** children);

void create_global_block();

node_t* create_func_def_node(int line_no, char* return_type, char* identifier, node_t* arg_def_block, node_t* stmnt_block);
node_t* create_func_call_node(int line_no, char* identifier, node_t* arg_block);
node_t* create_arg_def_node(int line_no, char* type, char* identifier);
node_t* create_assign_node(int line_no, node_t* dest, node_t* expr, char* op);
node_t* create_declaration_node(int line_no, char* type, char* identifier);
node_t* create_declaration_with_assign_node(int line_no, char* type, char* identifier, node_t* expr);
node_t* create_array_access_node(int line_no, char* identifier, node_t* accessor);
node_t* create_array_accessor_node(int line_no, node_t* expr);
node_t* create_array_dec_node(int line_no, char* identifier, int size, node_t* literal_block);
node_t* create_array_dim_node(int line_no, double num);
node_t* create_multi_dim_array_dec_node(int line_no, char* identifier, node_t* dimensions, node_t* literal_block);
node_t* create_struct_def_node(int line_no, char* identifier, node_t* member_block);
node_t* create_struct_init_node(int line_no, char* var_type, char* identifier, char* obj_type);
node_t* create_symbol_node(int line_no, char* identifier);
node_t* create_member_func_call_node(int line_no, node_t* obj_access, node_t* func_call);

node_t* create_if_statement_node(int line_no, node_t* rel_expr, node_t* stmnt_block, node_t* elseif_block, node_t* else_stmnt);
node_t* create_elseif_statement_node(int line_no, node_t* rel_expr, node_t* stmnt_block);
node_t* create_else_statement_node(int line_no, node_t* stmnt_block);

node_t* create_for_loop_node(int line_no, node_t* assign_stmnt, node_t* rel_expr, node_t* inc_stmnt, node_t* stmnt_block);
node_t* create_while_loop_node(int line_no, node_t* rel_expr, node_t* stmnt_block);
node_t* create_do_while_loop_node(int line_no, node_t* stmnt_block, node_t* rel_expr);
node_t* create_do_until_loop_node(int line_no, node_t* stmnt_block, node_t* rel_expr);

node_t* create_labelmaker_node(int line_no, char* identifier);
node_t* create_goto_statement_node(int line_no, char* identifier);
node_t* create_break_statement_node(int line_no);
node_t* create_continue_statement_node(int line_no);
node_t* create_return_statement_node(int line_no, node_t* expr);

node_t* create_bin_expr_node(int line_no, node_t* left_node, node_t* right_node, char* op);
node_t* create_primary_node(int line_no, primary_type_e val_type, void* val);
node_t* create_postfix_node(int line_no, char* identifier, char* op);

node_t* handle_parent_block(int line_no, node_t* parent, node_type_e parent_type, node_t* child);
node_t* set_expr_paren(int line_no, node_t* node);
node_t* set_expr_unary(int line_no, node_t* node, char* unary);

void tree_handle_error(int line_no, char* err);

#endif
