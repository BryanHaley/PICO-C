/* tree_checker.c
 * 
 * Some statements may not be natively supported in the target language
 * (e.g. continue or switch statements in lua), so they may need to be replaced with
 * other available statements. Sometimes this can be done trivially in code generation,
 * like with complex for loops being implemented as while loops, but non-trivial cases
 * should be handled here.
 */

#include <stdlib.h>
#include "tree_checker.h"
#include "tree_handler.h"
#include "code_gen.h"

void check_tree(tree_t* syntax_tree)
{
    check_node(syntax_tree->global_block);
}

void check_node(node_t* node)
{
    switch(node->node_type)
    {
        case (NODE_FUNC_DEF):
            check_func_def(node);
            break;
        case (NODE_FUNC_CALL):
            check_func_call(node);
            break;
        case (NODE_ARG_DEF):
            check_arg_def(node);
            break;
        case (NODE_BIN_EXPR):
            check_bin_expr(node);
            break;
        case (NODE_PRI):
            check_primary(node);
            break;
        case (NODE_NULL_STMNT):
            // do nothing
            break;
        case (NODE_ASSIGN):
            check_assignment(node);
            break;
        case (NODE_DEC):
            check_declaration(node);
            break;
        case (NODE_DEC_W_ASSIGN):
            check_declaration_with_assign(node);
            break;
        case (NODE_POSTFIX):
            check_postfix(node);
            break;
        case (NODE_ARR_ACCESS):
            check_array_access(node);
            break;
        case (NODE_ARR_ACCESSOR):
            check_array_accessor(node);
            break;
        case (NODE_ARR_DEC):
            check_array_declaration(node);
            break;
        case (NODE_ARR_DIM):
            check_array_dim(node);
            break;
        case (NODE_ARR_MULTI_DIM_DEC):
            check_mutli_dim_array_dec(node);
            break;
        case (NODE_STRUCT_DEF):
            check_struct_definition(node);
            break;
        case (NODE_STRUCT_INIT):
            check_struct_initialization(node);
            break;
        case (NODE_SYMBOL):
            check_symbol(node);
            break;
        case (NODE_METHOD_CALL):
            check_method_call(node);
            break;
        case (NODE_IF_STMNT):
            check_if_stmnt(node);
            break;
        case (NODE_ELSEIF_STMNT):
            check_elseif_stmnt(node);
            break;
        case (NODE_ELSE_STMNT):
            check_else_stmnt(node);
            break;
        case (NODE_FOR_LOOP):
            check_for_loop(node);
            break;
        case (NODE_WHILE_LOOP):
            check_while_loop(node);
            break;
        case (NODE_DO_WHILE_LOOP):
            check_do_while_loop(node);
            break;
        case (NODE_DO_UNTIL_LOOP):
            check_do_until_loop(node);
            break;
        case (NODE_LABELMAKER):
            check_labelmaker(node);
            break;
        case (NODE_GOTO):
            check_goto_statement(node);
            break;
        case (NODE_BREAK):
            check_break_statement(node);
            break;
        case (NODE_RETURN):
            check_return_statement(node);
            break;
        case (NODE_CONTINUE):
            check_continue_statement(node);
            break;
        default:
            check_parent_block(node);
            break;
    }
}

void check_parent_block(node_t* parent)
{
    if (parent == NULL) { return; }

    parent_block_data* parent_data = (parent_block_data*) parent->data;

    for (int i = 0; i < parent_data->num_children; i++)
    {
        check_node(parent_data->children[i]);
    }
}

void check_func_def(node_t* node)
{
    if (node == NULL) { return; }

    func_def_data* data = (func_def_data*) node->data;

    if (data->arg_def_block != NULL)
    { 
        check_node(data->arg_def_block);
    }
    
    if (data->statement_block != NULL)
    { 
        check_node(data->statement_block); 
    }
}

void check_arg_def(node_t* node)
{
    /*if (node == NULL) { return; }
    
    arg_def_data* data = (arg_def_data*) node->data;*/

    return;
}

void check_func_call(node_t* node)
{
    if (node == NULL) { return; }
    
    func_call_data* data = (func_call_data*) node->data;

    if (data->arg_block != NULL)
    {
        check_node(data->arg_block);
    }
}

void check_bin_expr(node_t* node)
{
    if (node == NULL) { return; }
    
    bin_expr_data* data = (bin_expr_data*) node->data;

    if (data->left_node != NULL)
    {
        check_node(data->left_node);
    }

    if (data->right_node != NULL)
    {
        check_node(data->right_node);
    }
}

void check_primary(node_t* node)
{
    /*if (node == NULL) { return; }
    
    primary_data* data = (primary_data*) node->data;*/

    return;
}

void check_assignment(node_t* node)
{
    if (node == NULL) { return; }

    assign_data* data = (assign_data*) node->data;

    if (data->dest != NULL)
    { 
        check_node(data->dest); 
    }

    if (data->expr != NULL)
    { 
        check_node(data->expr); 
    }
}

void check_declaration(node_t* node)
{
    /*if (node == NULL) { return; }

    declaration_data* data = (declaration_data*) node->data;*/

    return;
}

void check_declaration_with_assign(node_t* node)
{
    if (node == NULL) { return; }

    declaration_with_assign_data* data = (declaration_with_assign_data*) node->data;

    if (data->expr != NULL)
    { 
        check_node(data->expr); 
    }
}

void check_postfix(node_t* node)
{
    /*if (node == NULL) { return; }

    postfix_data* data = (postfix_data*) node->data;*/

    return;
}

void check_array_access(node_t* node)
{
    if (node == NULL) { return; }

    array_access_data* data = (array_access_data*) node->data;

    if (data->accessors != NULL)
    { 
        check_node(data->accessors); 
    }
}

void check_array_accessor(node_t* node)
{
    if (node == NULL) { return; }

    array_accessor_data* data = (array_accessor_data*) node->data;

    if (data->expr != NULL)
    { 
        check_node(data->expr); 
    }
}

void check_array_declaration(node_t* node)
{
    if (node == NULL) { return; }

    array_dec_data* data = (array_dec_data*) node->data;

    if (data->literal_block != NULL)
    { 
        check_node(data->literal_block); 
    }
}

void check_struct_definition(node_t* node)
{
    if (node == NULL) { return; }

    struct_def_data* data = (struct_def_data*) node->data;

    if (data->member_block != NULL)
    { 
        check_node(data->member_block); 
    }
}

void check_struct_initialization(node_t* node)
{
    /*if (node == NULL) { return; }

    struct_init_data* data = (struct_init_data*) node->data;*/

    return;
}

void check_array_dim(node_t* node)
{
    /*if (node == NULL) { return; }

    array_dim_data* data = (array_dim_data*) node->data;*/

    return;
}

void check_mutli_dim_array_dec(node_t* node)
{
    if (node == NULL) { return; }

    array_multidim_dec_data* data = (array_multidim_dec_data*) node->data;

    if (data->dimensions != NULL)
    { 
        check_node(data->dimensions); 
    }

    if (data->literal_block != NULL)
    { 
        check_node(data->literal_block); 
    }
}

void check_symbol(node_t* node)
{
    /*if (node == NULL) { return; }

    symbol_data* data = (symbol_data*) node->data;*/

    return;
}

void check_method_call(node_t* node)
{
    if (node == NULL) { return; }

    method_call_data* data = (method_call_data*) node->data;

    if (data->obj_access != NULL)
    { 
        check_node(data->obj_access); 
    }

    if (data->func_call != NULL)
    { 
        check_node(data->func_call); 
    }
}

void check_if_stmnt(node_t* node)
{
    if (node == NULL) { return; }

    if_stmnt_data* data = (if_stmnt_data*) node->data;

    if (data->rel_expr != NULL)     { check_node(data->rel_expr); }
    if (data->stmnt_block != NULL)  { check_node(data->stmnt_block); }
    if (data->elseif_block != NULL) { check_node(data->elseif_block); }
    if (data->else_stmnt != NULL)   { check_node(data->else_stmnt); }
}

void check_elseif_stmnt(node_t* node)
{
    if (node == NULL) { return; }

    elseif_stmnt_data* data = (elseif_stmnt_data*) node->data;

    if (data->rel_expr != NULL)    { check_node(data->rel_expr); }
    if (data->stmnt_block != NULL) { check_node(data->stmnt_block); }
}

void check_else_stmnt(node_t* node)
{
    if (node == NULL) { return; }

    else_stmnt_data* data = (else_stmnt_data*) node->data;

    if (data->stmnt_block != NULL) { check_node(data->stmnt_block); }
}

void check_for_loop(node_t* node)
{
    if (node == NULL) { return; }

    for_loop_data* data = (for_loop_data*) node->data;

    if (data->assign_stmnt != NULL) { check_node(data->assign_stmnt); }
    if (data->rel_expr != NULL)     { check_node(data->rel_expr); }
    if (data->inc_stmnt != NULL)    { check_node(data->inc_stmnt); }
    if (data->stmnt_block != NULL)  { check_node(data->stmnt_block); }
}

void check_while_loop(node_t* node)
{
    if (node == NULL) { return; }

    while_loop_data* data = (while_loop_data*) node->data;

    if (data->rel_expr != NULL)    { check_node(data->rel_expr); }
    if (data->stmnt_block != NULL) { check_node(data->stmnt_block); }
}

void check_do_while_loop(node_t* node)
{
    if (node == NULL) { return; }

    do_while_loop_data* data = (do_while_loop_data*) node->data;

    if (data->rel_expr != NULL)    { check_node(data->rel_expr); }
    if (data->stmnt_block != NULL) { check_node(data->stmnt_block); }
}

void check_do_until_loop(node_t* node)
{
    if (node == NULL) { return; }

    do_until_loop_data* data = (do_until_loop_data*) node->data;

    if (data->rel_expr != NULL)    { check_node(data->rel_expr); }
    if (data->stmnt_block != NULL) { check_node(data->stmnt_block); }
}

void check_labelmaker(node_t* node)
{
    /*if (node == NULL) { return; }

    labelmaker_data* data = (labelmaker_data*) node->data;*/

    return;
}

void check_goto_statement(node_t* node)
{
    /*if (node == NULL) { return; }

    goto_statement_data* data = (goto_statement_data*) node->data;*/

    return;
}

void check_break_statement(node_t* node)
{
    return;
}

void check_return_statement(node_t* node)
{
    if (node == NULL) { return; }

    return_statement_data* data = (return_statement_data*) node->data;

    if (data->expr != NULL) { check_node(data->expr); }
}

void check_continue_statement(node_t* node)
{
    if (node == NULL) { return; }

    /* Continue statements can be emulated in Lua using a label maker statement and
     * a goto statement. For for loops and do while/continue loops, we want the
     * continue label to go at the bottom of the loop so the increment and condition
     * check operations happen properly. For while loops, we need the label to go
     * outside and above the loop so that the condition check happens.
     */

    // First step is to find the closest applicable loop node by following parents
    node_t* loop = NULL;
    node_t* current = node;

    while (loop == NULL)
    {
        current = current->parent;
        if (current == NULL || current == syntax_tree->global_block) { break; }

        if (current->node_type == NODE_FOR_LOOP || current->node_type == NODE_WHILE_LOOP ||
            current->node_type == NODE_DO_WHILE_LOOP || current->node_type == NODE_DO_UNTIL_LOOP)
        {
            loop = current;
        }
    }

    if (loop == NULL)
    {
        tree_handle_error(node->line_no, "Continue statement not used within loop.");
        return;
    }

    /* Instead of doing the potentially complex operation of inserting a label maker
     * statement into an arbitrary point of the syntax tree, I opted to simply add a
     * continue node pointer to the node data structure. Just let it be handled in code gen. 
     * This increases repeated code but is less error-prone, as we'd have to handle a
     * lot of edge cases here and future changes to the language might require this
     * function to change too.
     */

    loop->continue_statement = node;
    continue_statement_data* continue_data = (continue_statement_data*) node->data;

    // create a unique identifier for the continue label
    continue_data->identifier = get_unique_name_with_prefix("_PCC_CONTINUE_");
}