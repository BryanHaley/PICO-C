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
        case (NODE_FUNC_CALL):
            generate_func_call(node);
            break;
        case (NODE_ARG_DEF_BLOCK):
            generate_parent_block(node, true);
            break;
        case (NODE_ARG_DEF):
            generate_arg_def(node);
            break;
        case (NODE_ARG_BLOCK):
            generate_parent_block(node, true);
            break;
        case (NODE_NULL_STMNT):
            //fprintf(output_file, ";");
            break;
        default:
            generate_parent_block(node, false);
            break;
    }
}

void generate_parent_block(node_t* parent, bool comma_delim)
{
    parent_block_data* parent_data = (parent_block_data*) parent->data;

    for (int i = 0; i < parent_data->num_children; i++)
    {
        generate_node(parent_data->children[i]);

        if (comma_delim && i != parent_data->num_children-1)
        {
            fprintf(output_file, ", ");
        }
    }
}

void generate_func_def(node_t* node)
{
    func_def_data* data = (func_def_data*) node->data;

    fprintf(output_file, "\nfunction %s ( ", data->identifier);

    if (data->arg_def_block != NULL)
    { 
        generate_node(data->arg_def_block);
    }

    fprintf(output_file, " )\n");
    
    if (data->statement_block != NULL)
    { 
        generate_parent_block(data->statement_block); 
    }

    fprintf(output_file, "\nend");
}

void generate_arg_def(node_t* node)
{
    arg_def_data* data = (arg_def_data*) node->data;

    fprintf(output_file, "%s", data->identifier);
}

void generate_func_call(node_t* node)
{
    func_call_data* data = (func_call_data*) node->data;

    fprintf(output_file, "%s(", data->identifier);

    // TODO: implement expression code generation
    //generate_node(data->arg_block);

    fprintf(output_file, ")\n");
}