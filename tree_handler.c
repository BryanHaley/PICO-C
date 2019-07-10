#include <stdlib.h>
#include "parse.h"
#include "tree_handler.h"

node_t* create_node(node_type_e node_type)
{
    node_t *node = (node_t*) calloc(1, sizeof(node_t));
    node->node_type = node_type;

    switch (node_type)
    {
        case (NODE_FUNC_DEF):
            node->data = calloc(1, sizeof(func_def_data));
            break;
        case (NODE_FUNC_CALL):
            node->data = calloc(1, sizeof(func_call_data));
            break;
        /*case (NODE_ARG):
            node->data = calloc(1, sizeof(arg_data));
            break;*/
        case (NODE_ARG_DEF):
            node->data = calloc(1, sizeof(arg_def_data));
            break;
        case (NODE_ASSIGN):
            node->data = calloc(1, sizeof(assign_data));
            break;
        case (NODE_DEC):
            node->data = calloc(1, sizeof(declaration_data));
            break;
        case (NODE_DEC_W_ASSIGN):
            node->data = calloc(1, sizeof(declaration_with_assign_data));
            break;
        case (NODE_PRI):
            node->data = calloc(1, sizeof(primary_data));
            break;
        case (NODE_BIN_EXPR):
            node->data = calloc(1, sizeof(bin_expr_data));
        case (NODE_NULL_STMNT):
            // no data
            break;
        default:
            node->data = calloc(1, sizeof(parent_block_data));
            break;
    }

    return node;
}

parent_block_data* add_child_to_parent_block(node_t* parent, 
                                            int num_new_children, node_t** children)
{
    parent_block_data *parent_data = (parent_block_data*) parent->data;

    if (parent_data->num_children == 0)
    {
        parent_data->num_children = num_new_children;
        parent_data->children = (node_t**) calloc(num_new_children, sizeof(node_t*));

        for (int i = 0; i < num_new_children; i++)
        {
            parent_data->children[i] = children[i];
            parent_data->children[i]->parent = parent;
        }
    }

    else
    {
        int total_children = parent_data->num_children+num_new_children;
        int startAt = parent_data->num_children;

        parent_data->num_children = total_children;
        parent_data->children = (node_t**) realloc(parent_data->children, 
            total_children*sizeof(node_t*));

        for (int i = 0; i < num_new_children; i++)
        {
            parent_data->children[i+startAt] = children[i];
            parent_data->children[i+startAt]->parent = parent;
        }
    }

    return parent_data;
}

void create_global_block()
{
    syntax_tree = (tree_t*) calloc(1, sizeof(tree_t));
    
    node_t *global_block = create_node(NODE_GLOBAL_BLOCK);

    syntax_tree->global_block = global_block;
}

void add_func_def_to_global_block(node_t* node)
{
    node_t* child[1] = { node };
    add_child_to_parent_block(syntax_tree->global_block, 1, child);
}

node_t* create_func_def_node(char* return_type, char* identifier, node_t* arg_def_block, 
                             node_t* stmnt_block)
{
    node_t* node = create_node(NODE_FUNC_DEF);
    func_def_data* data = (func_def_data*) node->data;

    data->return_type = return_type;
    data->identifier = identifier;
    data->arg_def_block = arg_def_block;
    data->statement_block = stmnt_block;

    arg_def_block->parent = node;
    stmnt_block->parent = node;

    return node;
}

node_t* create_func_call_node(char* identifier, node_t* arg_block)
{
    node_t* node = create_node(NODE_FUNC_CALL);
    func_call_data* data = (func_call_data*) node->data;

    data->identifier = identifier;
    data->arg_block = arg_block;

    arg_block->parent = node;

    return node;
}

node_t* create_arg_def_node(char* type, char* identifier)
{
    node_t* node = create_node(NODE_ARG_DEF);
    arg_def_data* data = (arg_def_data*) node->data;

    data->type = type;
    data->identifier = identifier;

    node_t* child[1] = { node };
    add_child_to_parent_block(current_arg_def_block, 1, child);

    return node;
}

node_t* create_assign_node(char* identifier, node_t* expr)
{
    node_t* node = create_node(NODE_ASSIGN);
    assign_data* data = (assign_data*) node->data;

    data->identifier = identifier;
    data->expr = expr;

    expr->parent = node;

    return node;
}

node_t* create_declaration_node(char* type, char* identifier)
{
    node_t* node = create_node(NODE_DEC);
    declaration_data* data = (declaration_data*) node->data;

    data->type = type;
    data->identifier = identifier;

    return node;
}

node_t* create_declaration_with_assign_node(char* type, char* identifier, node_t* expr)
{
    node_t* node = create_node(NODE_DEC_W_ASSIGN);
    declaration_with_assign_data* data = (declaration_with_assign_data*) node->data;

    data->type = type;
    data->identifier = identifier;
    data->expr = expr;

    expr->parent = node;

    return node;
}

node_t* create_bin_expr_node(node_t* left_node, node_t* right_node, char op)
{
    node_t* node = create_node(NODE_BIN_EXPR);
    bin_expr_data* data = (bin_expr_data*) node->data;

    data->left_node = left_node;
    data->right_node = right_node;
    data->op = op;

    left_node->parent = node;
    right_node->parent = node;

    return node;
}

node_t* create_primary_node_num(primary_type_e val_type, double val)
{
    node_t* node = create_node(NODE_PRI);
    primary_data* data = (primary_data*) node->data;

    data->val_type = val_type;
    data->val.numValue = val;

    return node;
}

node_t* create_primary_node_str(primary_type_e val_type, char* val)
{
    node_t* node = create_node(NODE_PRI);
    primary_data* data = (primary_data*) node->data;

    data->val_type = val_type;
    data->val.strValue = val;

    return node;
}

node_t* create_primary_node_nde(primary_type_e val_type, node_t* val)
{
    node_t* node = create_node(NODE_PRI);
    primary_data* data = (primary_data*) node->data;

    data->val_type = val_type;
    data->val.nodeValue = val;

    val->parent = node;

    return node;
}

void handle_arg_node(node_t* node)
{
    node_t* child[1] = { node };
    add_child_to_parent_block(current_arg_block, 1, child);
}

void handle_stmnt_node(node_t* node)
{
    node_t* child[1] = { node };
    add_child_to_parent_block(current_statement_block, 1, child);
}

void handle_null_stmnt_node()
{
    node_t* child[1] = { create_node(NODE_NULL_STMNT) };
    add_child_to_parent_block(current_statement_block, 1, child);
}

void clear_arg_def_block()
{
    node_t* node = create_node(NODE_ARG_DEF_BLOCK);
    current_arg_def_block = node;
}

void clear_statement_block()
{
    node_t* node = create_node(NODE_STMNT_BLOCK);
    current_statement_block = node;
}

void clear_arg_block()
{
    node_t* node = create_node(NODE_ARG_BLOCK);
    current_arg_block = node;
}