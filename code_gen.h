#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include "tree_handler.h"

FILE* output_file;

void generate_code(tree_t* syntax_tree);
void generate_node(node_t* node);
void generate_parent_block(node_t* parent);
void generate_func_def(node_t* node);
void generate_arg_def_block(node_t* node);
void generate_arg_def(node_t* node);

#endif
