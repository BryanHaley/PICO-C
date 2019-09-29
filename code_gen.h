#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include "tree_handler.h"

#define INDENT_TOKEN "    "

FILE* output_file;
int indent_level;

void generate_code(FILE* out, tree_t* syntax_tree);
void generate_node(node_t* node);
void generate_parent_block(node_t* parent, bool comma_delim);
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
void generate_array_accessor(node_t* node);
void generate_array_declaration(node_t* node);

#endif
