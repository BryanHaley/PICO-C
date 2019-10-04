#include <stdlib.h>
#include <string.h>
#include "pcc_grammar.h"
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
        case (NODE_POSTFIX):
            node->data = calloc(1, sizeof(postfix_data));
            break;
        case (NODE_ARR_ACCESS):
            node->data = calloc(1, sizeof(array_access_data));
            break;
        case (NODE_ARR_ACCESSOR):
            node->data = calloc(1, sizeof(array_accessor_data));
            break;
        case (NODE_ARR_DIM):
            node->data = calloc(1, sizeof(array_dim_data));
            break;
        case (NODE_ARR_DEC):
            node->data = calloc(1, sizeof(array_dec_data));
            break;
        case (NODE_ARR_MULTI_DIM_DEC):
            node->data = calloc(1, sizeof(array_multidim_dec_data));
            break;
        case (NODE_STRUCT_DEF):
            node->data = calloc(1, sizeof(struct_def_data));
            break;
        case (NODE_STRUCT_INIT):
            node->data = calloc(1, sizeof(struct_init_data));
            break;
        case (NODE_SYMBOL):
            node->data = calloc(1, sizeof(symbol_data));
            break;
        default:
            node->data = calloc(1, sizeof(parent_block_data));
            break;
    }

    return node;
}

parent_block_data* add_child_to_parent_block(node_t* parent, node_t* child)
{
    node_t* child_arr[1] = { child };
    return add_children_to_parent_block(parent, 1, child_arr);
}

parent_block_data* add_children_to_parent_block(node_t* parent, 
                                                int num_new_children, node_t** children)
{
    if (parent == NULL || children == NULL || children[0] == NULL)
    {
        return NULL;
    }

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

node_t* create_func_def_node(char* return_type, char* identifier, node_t* arg_def_block, 
                             node_t* stmnt_block)
{
    node_t* node = create_node(NODE_FUNC_DEF);
    func_def_data* data = (func_def_data*) node->data;

    data->return_type = return_type;
    data->identifier = identifier;
    data->arg_def_block = arg_def_block;
    data->statement_block = stmnt_block;

    if (arg_def_block != NULL) { arg_def_block->parent = node; }
    if (stmnt_block != NULL) { stmnt_block->parent = node; }

    return node;
}

node_t* create_func_call_node(char* identifier, node_t* arg_block)
{
    node_t* node = create_node(NODE_FUNC_CALL);
    func_call_data* data = (func_call_data*) node->data;

    data->identifier = identifier;
    data->arg_block = arg_block;

    if (arg_block->parent != NULL) { arg_block->parent = node; }

    return node;
}

node_t* create_arg_def_node(char* type, char* identifier)
{
    node_t* node = create_node(NODE_ARG_DEF);
    arg_def_data* data = (arg_def_data*) node->data;

    data->type = type;
    data->identifier = identifier;

    return node;
}

node_t* create_assign_node(node_t* dest, node_t* expr, char* op)
{
    node_t* node = create_node(NODE_ASSIGN);
    assign_data* data = (assign_data*) node->data;

    data->dest = dest;
    data->expr = expr;
    data->op = op;

    if (expr != NULL) { expr->parent = node; }

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

    if (expr != NULL) { expr->parent = node; }

    return node;
}

node_t* create_array_access_node(char* identifier, node_t* accessors)
{
    node_t* node = create_node(NODE_ARR_ACCESS);
    array_access_data* data = (array_access_data*) node->data;

    data->identifier = identifier;
    data->accessors = accessors;

    if (accessors != NULL) { accessors->parent = node; }

    return node;
}

node_t* create_array_accessor_node(node_t* expr)
{
    node_t* node = create_node(NODE_ARR_ACCESSOR);
    array_accessor_data* data = (array_accessor_data*) node->data;

    data->expr = expr;

    if (expr != NULL) { expr->parent = node; }

    return node;
}

node_t* create_array_dec_node(char* identifier, int size, node_t* literal_block)
{
    node_t* node = create_node(NODE_ARR_DEC);
    array_dec_data* data = (array_dec_data*) node->data;

    data->identifier = identifier;
    data->size = size;
    data->literal_block = literal_block;

    if (literal_block != NULL) { literal_block->parent = node; }

    return node;
}

node_t* create_bin_expr_node(node_t* left_node, node_t* right_node, char* op)
{
    node_t* node = create_node(NODE_BIN_EXPR);
    bin_expr_data* data = (bin_expr_data*) node->data;

    data->left_node = left_node;
    data->right_node = right_node;
    data->op = op;

    if (left_node != NULL) { left_node->parent = node; }
    if (right_node != NULL) { right_node->parent = node; }

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

    if (val != NULL) { val->parent = node; }

    return node;
}

node_t* create_postfix_node(char* identifier, char* op)
{
    node_t* node = create_node(NODE_POSTFIX);
    postfix_data* data = (postfix_data*) node->data;

    data->identifier = identifier;
    data->op = op;

    return node;
}

node_t* create_struct_def_node(char* identifier, node_t* member_block)
{
    node_t* node = create_node(NODE_STRUCT_DEF);
    struct_def_data* data = (struct_def_data*) node->data;

    data->identifier = identifier;
    data->member_block = member_block;

    if (member_block != NULL) { member_block->parent = node; }

    return node;
}

node_t* handle_parent_block(node_t* parent, node_type_e parent_type, node_t* child)
{
    if (parent == NULL)
    {
        parent = create_node(parent_type);
    }

    if (child != NULL)
    {
        add_child_to_parent_block(parent, child);
    }

    return parent;
}

node_t* set_expr_paren(node_t* node)
{
    if (node->node_type == NODE_BIN_EXPR)
    {
        bin_expr_data* data = (bin_expr_data*) node->data;
        data->in_parentheses = true;
        return node;
    }

    else if (node->node_type == NODE_PRI)
    {
        primary_data* data = (primary_data*) node->data;
        data->in_parentheses = true;
        return node;
    }

    else
    {
        /* error, unknown type */
        return node;
    }
}
node_t* set_expr_unary(node_t* node, char* unary)
{
    if (node->node_type == NODE_BIN_EXPR)
    {
        bin_expr_data* data = (bin_expr_data*) node->data;
        data->unary = unary;
        return node;
    }

    else if (node->node_type == NODE_PRI)
    {
        primary_data* data = (primary_data*) node->data;
        data->unary = unary;
        return node;
    }

    else
    {
        /* error, unknown type */
        return node;
    }
}

node_t* create_struct_init_node(char* var_type, char* identifier, char* obj_type)
{
    node_t* node = create_node(NODE_STRUCT_INIT);
    struct_init_data* data = (struct_init_data*) node->data;

    if (var_type == NULL)
    {
        var_type = obj_type;
    }

    else if (strcmp(var_type, obj_type) != 0)
    {
        /* error, wrong type */
        return node;
    }

    data->type = var_type;
    data->identifier = identifier;

    return node;
}

node_t* create_array_dim_node(double num)
{
    node_t* node = create_node(NODE_ARR_DIM);
    array_dim_data* data = (array_dim_data*) node->data;

    if ((int) num != num)
    {
        /* error, not an integer */
        return node;
    }

    data->num = (int) num;

    return node;
}

node_t* create_multi_dim_array_dec_node(char* identifier, node_t* dimensions,
                                        node_t* literal_block)
{
    node_t* node = create_node(NODE_ARR_MULTI_DIM_DEC);
    array_multidim_dec_data* data = (array_multidim_dec_data*) node->data;

    data->identifier = identifier;
    data->dimensions = dimensions;
    data->literal_block = literal_block;

    if (literal_block != NULL) { literal_block->parent = node; }

    return node;
}

node_t* create_symbol_node(char* identifier)
{
    node_t* node = create_node(NODE_SYMBOL);
    symbol_data* data = (symbol_data*) node->data;

    data->identifier = identifier;

    return node;
}