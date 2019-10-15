#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include "tree_handler.h"

#define INDENT_TOKEN "    "

FILE* output_file;
int indent_level;
int unique_name_suffix_num;

void code_gen_error(int line_no, char* err);
char* get_unique_name();
char* get_unique_name_with_prefix(char* prefix);
void print_indents_with_additional(int additional);
void print_indents();

void generate_code(FILE* out, tree_t* syntax_tree);
void generate_node(node_t* node);
void generate_parent_block(node_t* parent, char* delim);
void generate_func_def(node_t* node);
void generate_func_call(node_t* node);
void generate_arg_def_block(node_t* node);
void generate_arg_def(node_t* node);
void generate_arg_block(node_t* node);
void generate_arg(node_t* node);
void generate_bin_expr(node_t* node);
void generate_primary(node_t* node);
void generate_assignment(node_t* node);
void generate_declaration(node_t* node);
void generate_declaration_with_assign(node_t* node);
void generate_postfix(node_t* node);
void generate_array_access(node_t* node);
void generate_array_accessor(node_t* node);
void generate_array_dim(node_t* node);
void generate_mutli_dim_array_dec(node_t* node);
void generate_array_declaration(node_t* node);
void generate_struct_definition(node_t* node);
void generate_struct_initialization(node_t* node);
void generate_symbol(node_t* node);
void generate_method_call(node_t* node);
void generate_if_stmnt(node_t* node);
void generate_elseif_stmnt(node_t* node);
void generate_else_stmnt(node_t* node);
void generate_for_loop(node_t* node);
void generate_while_loop(node_t* node);
void generate_do_while_loop(node_t* node);
void generate_do_until_loop(node_t* node);
void generate_labelmaker(node_t* node);
void generate_goto_statement(node_t* node);
void generate_break_statement(node_t* node);
void generate_return_statement(node_t* node);
void generate_continue_statement(node_t* node);
void generate_switch_statement(node_t* node);
void generate_case(node_t* node);

#endif
