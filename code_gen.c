#include <stdio.h>
#include "code_gen.h"
#include "tree_handler.h"
#include "ast.h"

void generate_code(tree_t* syntax_tree)
{
    output_file = stdout;

    generate_node(syntax_tree->global_block);

    fprintf(output_file, "\n");
}

void generate_node(node_t* node)
{
    switch(node->node_type)
    {
        case (NODE_FUNC_DEF):
            generate_func_def(node);
            break;
        case (NODE_ARG_DEF_BLOCK):
            generate_arg_def_block(node);
            break;
        case (NODE_ARG_DEF):
            generate_arg_def(node);
            break;
        default:
            generate_parent_block(node);
            break;
    }
}

void generate_parent_block(node_t* parent)
{
    parent_block_data* parent_data = (parent_block_data*) parent->data;

    for (int i = 0; i < parent_data->num_children; i++)
    {
        generate_node(parent_data->children[i]);
    }
}

void generate_func_def(node_t* node)
{
    func_def_data* data = (func_def_data*) node->data;

    fprintf(output_file, "\nfunction %s ( ", data->identifier->name);

    if (data->arg_def_block != NULL)
    { 
        generate_node(data->arg_def_block);
    }

    fprintf(output_file, " )\n");
    
    /*if (data->statement_block != NULL)
    { generate_statement_block(data->statement_block); }*/

    fprintf(output_file, "\nend");
}

void generate_arg_def_block(node_t* node)
{
    parent_block_data* parent_data = (parent_block_data*) node->data;

    for (int i = 0; i < parent_data->num_children; i++)
    {
        generate_node(parent_data->children[i]);

        if (i != parent_data->num_children-1)
        {
            fprintf(output_file, ", ");
        }
    } 
}

void generate_arg_def(node_t* node)
{
    arg_def_data* data = (arg_def_data*) node->data;

    fprintf(output_file, "%s", data->identifier->name);
}
