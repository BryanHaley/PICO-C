#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcc_grammar.h"
#include "tree_handler.h"

node_t* create_node(node_type_e node_type, int line_no)
{
    node_t *node = (node_t*) calloc(1, sizeof(node_t));
    node->node_type = node_type;
    node->line_no = line_no;

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
            break;
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
        case (NODE_METHOD_CALL):
            node->data = calloc(1, sizeof(method_call_data));
            break;
        case (NODE_IF_STMNT):
            node->data = calloc(1, sizeof(if_stmnt_data));
            break;
        case (NODE_ELSEIF_STMNT):
            node->data = calloc(1, sizeof(elseif_stmnt_data));
            break;
        case (NODE_ELSE_STMNT):
            node->data = calloc(1, sizeof(else_stmnt_data));
            break;
        case (NODE_FOR_LOOP):
            node->data = calloc(1, sizeof(for_loop_data));
            break;
        case (NODE_DO_WHILE_LOOP):
            node->data = calloc(1, sizeof(do_while_loop_data));
            break;
        case (NODE_WHILE_LOOP):
            node->data = calloc(1, sizeof(while_loop_data));
            break;
        case (NODE_LABELMAKER):
            node->data = calloc(1, sizeof(labelmaker_data));
            break;
        case (NODE_GOTO):
            node->data = calloc(1, sizeof(goto_statement_data));
            break;
        case (NODE_CONTINUE):
            node->data = calloc(1, sizeof(continue_statement_data));
            break;
        case (NODE_BREAK):
            node->data = NULL;
            break;
        case (NODE_NULL):
            node->data = NULL;
            break;
        case (NODE_SWITCH):
            node->data = calloc(1, sizeof(switch_statement_data));
            break;
        case (NODE_FAST_SWITCH):
            node->data = calloc(1, sizeof(fast_switch_data));
            break;
        case (NODE_CASE):
            node->data = calloc(1, sizeof(case_data));
            break;
        case (NODE_FSWITCH_CALL):
            node->data = calloc(1, sizeof(fswitch_call_data));
            break;
        default:
            node->data = calloc(1, sizeof(parent_block_data));
            break;
    }

    return node;
}

void tree_handle_error(int line_no, char* err)
{
    fprintf(stderr, "\nERROR in line %d: %s\n", line_no, err);
    err_in_tree = true;
    return;
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
    
    node_t *global_block = create_node(NODE_GLOBAL_BLOCK, 0);

    syntax_tree->global_block = global_block;
}

node_t* create_func_def_node(int line_no, char* return_type, char* identifier,
                             node_t* arg_def_block, node_t* stmnt_block)
{
    node_t* node = create_node(NODE_FUNC_DEF, line_no);
    func_def_data* data = (func_def_data*) node->data;

    data->return_type     = return_type;
    data->identifier      = identifier;
    data->arg_def_block   = arg_def_block;
    data->statement_block = stmnt_block;

    if (arg_def_block != NULL) { arg_def_block->parent = node; }
    if (stmnt_block != NULL)   { stmnt_block->parent = node; }

    return node;
}

node_t* create_func_call_node(int line_no, char* identifier, node_t* arg_block)
{
    node_t* node = create_node(NODE_FUNC_CALL, line_no);
    func_call_data* data = (func_call_data*) node->data;

    data->identifier = identifier;
    data->arg_block  = arg_block;

    if (arg_block != NULL) { arg_block->parent = node; }

    return node;
}

node_t* create_arg_def_node(int line_no, char* type, char* identifier)
{
    node_t* node = create_node(NODE_ARG_DEF, line_no);
    arg_def_data* data = (arg_def_data*) node->data;

    data->type       = type;
    data->identifier = identifier;

    return node;
}

node_t* create_assign_node(int line_no, node_t* dest, node_t* expr, char* op)
{
    node_t* node = create_node(NODE_ASSIGN, line_no);
    assign_data* data = (assign_data*) node->data;

    data->dest = dest;
    data->expr = expr;
    data->op   = op;

    if (dest != NULL) { dest->parent = node; }
    if (expr != NULL) { expr->parent = node; }

    return node;
}

node_t* create_declaration_node(int line_no, char* type, char* identifier)
{
    node_t* node = create_node(NODE_DEC, line_no);
    declaration_data* data = (declaration_data*) node->data;

    data->type       = type;
    data->identifier = identifier;

    return node;
}

node_t* create_declaration_with_assign_node(int line_no, char* type, char* identifier,
                                            node_t* expr)
{
    node_t* node = create_node(NODE_DEC_W_ASSIGN, line_no);
    declaration_with_assign_data* data = (declaration_with_assign_data*) node->data;

    data->type       = type;
    data->identifier = identifier;
    data->expr       = expr;

    if (expr != NULL) { expr->parent = node; }

    return node;
}

node_t* create_array_access_node(int line_no, char* identifier, node_t* accessors)
{
    node_t* node = create_node(NODE_ARR_ACCESS, line_no);
    array_access_data* data = (array_access_data*) node->data;

    data->identifier = identifier;
    data->accessors  = accessors;

    if (accessors != NULL) { accessors->parent = node; }

    return node;
}

node_t* create_array_accessor_node(int line_no, node_t* expr)
{
    node_t* node = create_node(NODE_ARR_ACCESSOR, line_no);
    array_accessor_data* data = (array_accessor_data*) node->data;

    data->expr = expr;

    if (expr != NULL) { expr->parent = node; }

    return node;
}

node_t* create_array_dec_node(int line_no, char* identifier, int size, node_t* literal_block)
{
    node_t* node = create_node(NODE_ARR_DEC, line_no);
    array_dec_data* data = (array_dec_data*) node->data;

    data->identifier    = identifier;
    data->size          = size;
    data->literal_block = literal_block;

    if (literal_block != NULL) { literal_block->parent = node; }

    return node;
}

node_t* create_bin_expr_node(int line_no, node_t* left_node, node_t* right_node, char* op)
{
    node_t* node = create_node(NODE_BIN_EXPR, line_no);
    bin_expr_data* data = (bin_expr_data*) node->data;

    data->left_node  = left_node;
    data->right_node = right_node;
    data->op         = op;

    if (left_node != NULL)  { left_node->parent = node; }
    if (right_node != NULL) { right_node->parent = node; }

    return node;
}

node_t* create_primary_node(int line_no, primary_type_e val_type, void* val)
{
    node_t* node = create_node(NODE_PRI, line_no);
    primary_data* data = (primary_data*) node->data;

    data->val_type = val_type;

    switch (val_type)
    {
        case (PRI_LITERAL_NUM):
            data->val.numValue = *((double*) val);
            break;
        case (PRI_LITERAL_STR):
            data->val.strValue = (char*) val;
            break;
        case (PRI_LITERAL_BOOL):
            data->val.boolValue = *((bool*) val);
            break;
        default:
            tree_handle_error(line_no, "Unknown primary type.");
            return NULL;
    }

    return node;
}

node_t* create_postfix_node(int line_no, char* identifier, char* op)
{
    node_t* node = create_node(NODE_POSTFIX, line_no);
    postfix_data* data = (postfix_data*) node->data;

    data->identifier = identifier;
    data->op = op;

    return node;
}

node_t* create_struct_def_node(int line_no, char* identifier, node_t* member_block)
{
    node_t* node = create_node(NODE_STRUCT_DEF, line_no);
    struct_def_data* data = (struct_def_data*) node->data;

    data->identifier   = identifier;
    data->member_block = member_block;

    if (member_block != NULL) { member_block->parent = node; }

    return node;
}

node_t* create_struct_init_node(int line_no, char* obj_type, node_t* func_call)
{
    node_t* node = create_node(NODE_STRUCT_INIT, line_no);
    struct_init_data* data = (struct_init_data*) node->data;

    /*if (var_type == NULL)
    {
        var_type = obj_type;
    }

    else if (strcmp(var_type, obj_type) != 0)
    {
        tree_handle_error(line_no, "Mismatch in variable and object type.");
        return NULL;
    }*/

    data->type      = obj_type;
    data->func_call = func_call;

    return node;
}

node_t* create_array_dim_node(int line_no, double num)
{
    node_t* node = create_node(NODE_ARR_DIM, line_no);
    array_dim_data* data = (array_dim_data*) node->data;

    if ((int) num != num)
    {
        tree_handle_error(line_no, "Non-integer used in array brackets.");
        return NULL;
    }

    data->num = (int) num;

    return node;
}

node_t* create_multi_dim_array_dec_node(int line_no, char* identifier, node_t* dimensions,
                                        node_t* literal_block)
{
    node_t* node = create_node(NODE_ARR_MULTI_DIM_DEC, line_no);
    array_multidim_dec_data* data = (array_multidim_dec_data*) node->data;

    data->identifier    = identifier;
    data->dimensions    = dimensions;
    data->literal_block = literal_block;

    if (dimensions != NULL)    { dimensions->parent = node; }
    if (literal_block != NULL) { literal_block->parent = node; }

    return node;
}

node_t* create_symbol_node(int line_no, char* identifier)
{
    node_t* node = create_node(NODE_SYMBOL, line_no);
    symbol_data* data = (symbol_data*) node->data;

    data->identifier = identifier;

    return node;
}

node_t* create_member_func_call_node(int line_no, node_t* obj_access, node_t* func_call)
{
    node_t* node = create_node(NODE_METHOD_CALL, line_no);
    method_call_data* data = (method_call_data*) node->data;

    data->obj_access = obj_access;
    data->func_call = func_call;

    if (obj_access != NULL) { obj_access->parent = node; }

    return node;
}

node_t* create_if_statement_node(int line_no, node_t* rel_expr, node_t* stmnt_block,
                                 node_t* elseif_block, node_t* else_stmnt)
{
    node_t* node = create_node(NODE_IF_STMNT, line_no);
    if_stmnt_data* data = (if_stmnt_data*) node->data;

    data->rel_expr     = rel_expr;
    data->stmnt_block  = stmnt_block;
    data->elseif_block = elseif_block;
    data->else_stmnt   = else_stmnt;

    if (rel_expr != NULL)     { rel_expr->parent = node; }
    if (stmnt_block != NULL)  { stmnt_block->parent = node; }
    if (elseif_block != NULL) { elseif_block->parent = node; }
    if (else_stmnt != NULL)   { else_stmnt->parent = node; }

    return node;
}

node_t* create_elseif_statement_node(int line_no, node_t* rel_expr, node_t* stmnt_block)
{
    node_t* node = create_node(NODE_ELSEIF_STMNT, line_no);
    elseif_stmnt_data* data = (elseif_stmnt_data*) node->data;

    data->rel_expr    = rel_expr;
    data->stmnt_block = stmnt_block;

    if (rel_expr != NULL)    { rel_expr->parent = node; }
    if (stmnt_block != NULL) { stmnt_block->parent = node; }

    return node;
}

node_t* create_else_statement_node(int line_no, node_t* stmnt_block)
{
    node_t* node = create_node(NODE_ELSE_STMNT, line_no);
    else_stmnt_data* data = (else_stmnt_data*) node->data;

    data->stmnt_block = stmnt_block;

    if (stmnt_block != NULL) { stmnt_block->parent = node; }

    return node;
}

node_t* create_for_loop_node(int line_no, node_t* assign_stmnt, node_t* rel_expr,
                             node_t* inc_stmnt, node_t* stmnt_block)
{
    node_t* node = create_node(NODE_FOR_LOOP, line_no);
    for_loop_data* data = (for_loop_data*) node->data;

    data->assign_stmnt = assign_stmnt;
    data->rel_expr     = rel_expr;
    data->inc_stmnt    = inc_stmnt;
    data->stmnt_block  = stmnt_block;

    if (assign_stmnt != NULL) { assign_stmnt->parent = node; }
    if (rel_expr != NULL)     { rel_expr->parent = node; }
    if (inc_stmnt != NULL)    { inc_stmnt->parent = node; }
    if (stmnt_block != NULL)  { stmnt_block->parent = node; }

    return node;
}

node_t* create_while_loop_node(int line_no, node_t* rel_expr, node_t* stmnt_block)
{
    node_t* node = create_node(NODE_WHILE_LOOP, line_no);
    while_loop_data* data = (while_loop_data*) node->data;

    data->rel_expr     = rel_expr;
    data->stmnt_block  = stmnt_block;

    if (rel_expr != NULL)     { rel_expr->parent = node; }
    if (stmnt_block != NULL)  { stmnt_block->parent = node; }

    return node;
}

node_t* create_do_while_loop_node(int line_no, node_t* stmnt_block, node_t* rel_expr)
{
    node_t* node = create_node(NODE_DO_WHILE_LOOP, line_no);
    do_while_loop_data* data = (do_while_loop_data*) node->data;

    data->rel_expr     = rel_expr;
    data->stmnt_block  = stmnt_block;

    if (rel_expr != NULL)     { rel_expr->parent = node; }
    if (stmnt_block != NULL)  { stmnt_block->parent = node; }

    return node;
}

node_t* create_do_until_loop_node(int line_no, node_t* stmnt_block, node_t* rel_expr)
{
    node_t* node = create_node(NODE_DO_UNTIL_LOOP, line_no);
    do_until_loop_data* data = (do_until_loop_data*) node->data;

    data->rel_expr     = rel_expr;
    data->stmnt_block  = stmnt_block;

    if (rel_expr != NULL)     { rel_expr->parent = node; }
    if (stmnt_block != NULL)  { stmnt_block->parent = node; }

    return node;
}

node_t* create_labelmaker_node(int line_no, char* identifier)
{
    node_t* node = create_node(NODE_LABELMAKER, line_no);
    labelmaker_data* data = (labelmaker_data*) node->data;

    data->identifier = identifier;

    return node;
}

node_t* create_goto_statement_node(int line_no, char* identifier)
{
    node_t* node = create_node(NODE_GOTO, line_no);
    goto_statement_data* data = (goto_statement_data*) node->data;

    data->identifier = identifier;

    return node;
}

node_t* create_break_statement_node(int line_no)
{
    node_t* node = create_node(NODE_BREAK, line_no);
    return node;
}

node_t* create_continue_statement_node(int line_no)
{
    node_t* node = create_node(NODE_CONTINUE, line_no);
    return node;
}

node_t* create_return_statement_node(int line_no, node_t* expr)
{
    node_t* node = create_node(NODE_RETURN, line_no);
    return_statement_data* data = (return_statement_data*) node->data;

    data->expr = expr;

    if (expr != NULL) { expr->parent = node; }

    return node;
}

node_t* create_switch_statement_node(int line_no, node_t* expr, node_t* case_block)
{
    node_t* node = create_node(NODE_SWITCH, line_no);
    switch_statement_data* data = (switch_statement_data*) node->data;

    data->expr       = expr;
    data->case_block = case_block;

    if (expr != NULL)       { expr->parent = node; }
    if (case_block != NULL) { case_block->parent = node; }

    return node;
}

node_t* create_fast_switch_statement_node(int line_no, char* type, char* identifier, node_t* params, node_t* case_block)
{
    node_t* node = create_node(NODE_FAST_SWITCH, line_no);
    fast_switch_data* data = (fast_switch_data*) node->data;

    data->type       = type;
    data->identifier = identifier;
    data->params     = params;
    data->case_block = case_block;

    if (params != NULL)     { params->parent = node; }
    if (case_block != NULL) { case_block->parent = node; }

    return node;
}

node_t* create_fswitch_call_node(int line_no, char* identifier, node_t* expr, node_t* arg_block)
{
    node_t* node = create_node(NODE_FSWITCH_CALL, line_no);
    fswitch_call_data* data = (fswitch_call_data*) node->data;

    data->identifier = identifier;
    data->expr       = expr;
    data->arg_block  = arg_block;

    if (expr != NULL)      { expr->parent = node; }
    if (arg_block != NULL) { arg_block->parent = node; }

    return node;
}

node_t* create_case_node(int line_no, node_t* expr, node_t* stmnt_block)
{
    node_t* node = create_node(NODE_CASE, line_no);
    case_data* data = (case_data*) node->data;

    data->expr        = expr;
    data->stmnt_block = stmnt_block;

    if (expr != NULL)        { expr->parent = node; }
    if (stmnt_block != NULL) { stmnt_block->parent = node; }

    return node;
}

node_t* create_null_node(int line_no)
{
    node_t* node = create_node(NODE_NULL, line_no);
    return node;
}

node_t* handle_parent_block(int line_no, node_t* parent, node_type_e parent_type,
                            node_t* child)
{
    if (parent == NULL)
    {
        parent = create_node(parent_type, line_no);
    }

    if (child != NULL)
    {
        add_child_to_parent_block(parent, child);
    }

    return parent;
}

node_t* set_expr_paren(int line_no, node_t* node)
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
        tree_handle_error(line_no, "Attempted to put non-expression in parantheses.");
        return NULL;
    }
}
node_t* set_expr_unary(int line_no, node_t* node, char* unary)
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
        tree_handle_error(line_no, "Attempted to apply unary operation to non-expression.");
        return NULL;
    }
}