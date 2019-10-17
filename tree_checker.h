#ifndef TREE_CHECKER_H
#define TREE_CHECKER_H

#include <stdbool.h>
#include "ast.h"

void check_tree(tree_t* syntax_tree);
void check_node(node_t* node);

void check_parent_block(node_t* parent);
void check_func_def(node_t* node);
void check_arg_def(node_t* node);
void check_func_call(node_t* node);
void check_bin_expr(node_t* node);
void check_primary(node_t* node);
void check_assignment(node_t* node);
void check_declaration(node_t* node);
void check_declaration_with_assign(node_t* node);
void check_postfix(node_t* node);
void check_array_access(node_t* node);
void check_array_accessor(node_t* node);
void check_array_declaration(node_t* node);
void check_struct_definition(node_t* node);
void check_struct_initialization(node_t* node);
void check_array_dim(node_t* node);
void check_mutli_dim_array_dec(node_t* node);
void check_symbol(node_t* node);
void check_method_call(node_t* node);
void check_if_stmnt(node_t* node);
void check_elseif_stmnt(node_t* node);
void check_else_stmnt(node_t* node);
void check_for_loop(node_t* node);
void check_while_loop(node_t* node);
void check_do_while_loop(node_t* node);
void check_do_until_loop(node_t* node);
void check_labelmaker(node_t* node);
void check_goto_statement(node_t* node);
void check_break_statement(node_t* node);
void check_return_statement(node_t* node);
void check_continue_statement(node_t* node);
void check_switch_statement(node_t* node);
void check_fast_switch_statement(node_t* node);
void check_case(node_t* node);
void check_fswitch_call(node_t* node);
void check_obj_accessor_block(node_t* node);

#endif