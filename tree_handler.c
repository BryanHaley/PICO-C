#include <stdlib.h>
#include "parse.h"
#include "tree_handler.h"

node_t* create_node(node_t *parent, node_type_e node_type)
{
    node_t *node = (node_t*) calloc(1, sizeof(node_t));
    node->node_type = node_type;

    switch (node_type)
    {
        case (NODE_FUNC_DEF):
            node->data = calloc(1, sizeof(func_def_data));
            break;
        case (NODE_ARG_DEF):
            node->data = calloc(1, sizeof(arg_def_data));
            break;
        case (NODE_STMNT):
            node->data = calloc(1, sizeof(statement_data));
            break;
        case (NODE_ASSIGN):
            node->data = calloc(1, sizeof(assign_data));
            break;
        case (NODE_FUNC_CALL):
            node->data = calloc(1, sizeof(func_call_data));
            break;
        case (NODE_ARG):
            node->data = calloc(1, sizeof(arg_data));
            break;
        default:
            node->data = calloc(1, sizeof(parent_block_data));
            break;
    }

    node->parent = parent;
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

// TODO: Implement a symbol table to keep track of existing symbols.
//       For now, just return new symbols every time.
symbol_t* get_symbol_by_identifier(node_t *scope, char* identifier)
{
    // For now, assume var and just create a new symbol
    symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
    symbol->type = TYPE_VAR;
    symbol->name = identifier;
    symbol->val.numVal = 0;

    return symbol;
}

symbol_t* get_var_symbol(node_t* scope, char* name, double value, 
                         bool create_if_DNE, bool fail_if_exists)
{
    symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
    symbol->type = TYPE_VAR;
    symbol->name = name;
    symbol->val.numVal = value;

    return symbol;
}

symbol_t* get_pointer_symbol(node_t* scope, char* name, 
                             bool create_if_DNE, bool fail_if_exists)
{
    symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
    symbol->type = TYPE_POINTER;
    symbol->name = name;

    return symbol;
}

// Literals cannot be 'reused', so there's no need to do a table lookup or scope check
symbol_t* get_literal_num_symbol(double value)
{
    symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
    symbol->type = TYPE_LITERAL_NUM;
    symbol->val.numVal = value;

    return symbol;
}

symbol_t* get_literal_str_symbol(char* strVal)
{
    symbol_t* symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
    symbol->type = TYPE_LITERAL_STRING;
    symbol->val.strVal = strVal;

    return symbol;
}

symbol_t* get_literal_bool_symbol(bool boolVal)
{
    symbol_t* symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
    symbol->type = TYPE_LITERAL_BOOL;
    symbol->val.boolVal = boolVal;

    return symbol;
}

void handle_global_block()
{
    syntax_tree = (tree_t*) calloc(1, sizeof(tree_t));
    
    node_t *global_block = create_node(NULL, NODE_GLOBAL_BLOCK);

    syntax_tree->global_block = global_block;

    current_scope = global_block;
}

void handle_function_def(char* name, data_types_e return_type)
{
    if (current_scope != syntax_tree->global_block)
    {
        yyerror("Functions can only be defined in the global scope");
    }

    node_t* func_def_node = current_func_def;
    func_def_data* data = (func_def_data*) func_def_node->data;

    data->return_type = return_type;
    data->identifier = get_pointer_symbol(current_scope, name, true, true);
    data->arg_def_block = current_arg_def_block;
    data->statement_block = current_statement_block;

    current_arg_def_block->parent = func_def_node;
    current_statement_block->parent = func_def_node;

    node_t* new_gb_children[1] = { func_def_node };
    add_child_to_parent_block(syntax_tree->global_block, 1, new_gb_children);

    current_scope = func_def_node;
}

void handle_func_call(symbol_t* func)
{
    if (current_scope == syntax_tree->global_block)
    {
        yyerror("Functions cannot be called in the global scope");
    }

    node_t *func_call = create_node(current_scope, NODE_FUNC_CALL);
    func_call_data *data = (func_call_data*) func_call->data;

    data->identifier = func;
    data->argument_block = current_arg_block;
    current_arg_block->parent = func_call;

    current_func_call = func_call;
}

void handle_arg_def(data_types_e type, char* symbol_name)
{
    node_t *arg_def_node = create_node(current_arg_def_block, NODE_ARG_DEF);
    arg_def_data *arg_def_node_data = (arg_def_data*) arg_def_node->data;

    switch (type)
    {
        case (TYPE_VAR):
            arg_def_node_data->identifier = get_var_symbol(current_scope, symbol_name, 
                                                0, true, true);
            break;
        case (TYPE_POINTER):
            arg_def_node_data->identifier = get_pointer_symbol(current_scope, symbol_name, 
                                            true, true);
            break;
        default:
            yyerror("Unrecognized symbol type");
            break;
    }
    
    node_t* child[1] = { arg_def_node };
    add_child_to_parent_block(current_arg_def_block, 1, child);

    current_arg_def = arg_def_node;
}

void handle_assignment_to_identifier(char* iden_name_left, char* iden_name_right)
{
    node_t *assn_node = create_node(current_statement_block, NODE_ASSIGN);
    assign_data *assign_node_data = (assign_data*) assn_node->data;

    assign_node_data->left_operand = get_symbol_by_identifier(current_scope, iden_name_left);
    assign_node_data->right_operand.symbol = get_symbol_by_identifier(current_scope, 
                                                                iden_name_right);
    assign_node_data->right_operand_is_expression = false;

    node_t* child[1] = { assn_node };
    add_child_to_parent_block(current_statement_block, 1, child);

    current_assign = assn_node;
}

void handle_assignment_to_literal(char* iden_name, data_types_e type, symbol_value val)
{
    node_t *assn_node = create_node(current_statement_block, NODE_ASSIGN);
    assign_data *assign_node_data = (assign_data*) assn_node->data;

    assign_node_data->left_operand = get_symbol_by_identifier(current_scope, iden_name);
    
    switch (type)
    {
        case (TYPE_LITERAL_NUM):
            assign_node_data->right_operand.symbol = get_literal_num_symbol(val.numVal);
            break;
        case (TYPE_LITERAL_STRING):
            assign_node_data->right_operand.symbol = get_literal_str_symbol(val.strVal);
            break;
        case (TYPE_LITERAL_BOOL):
            assign_node_data->right_operand.symbol = get_literal_bool_symbol(val.boolVal);
            break;
        default:
            yyerror("Unknown literal type");
            break;
    }
    
    assign_node_data->right_operand_is_expression = false;

    node_t* child[1] = { assn_node };
    add_child_to_parent_block(current_statement_block, 1, child); 

    current_assign = assn_node;
}

node_t* handle_arg_def_block()
{
    node_t *arg_def_block = create_node(NULL, NODE_ARG_DEF_BLOCK);

    current_arg_def_block = arg_def_block;
    return arg_def_block;
}

node_t* handle_statement_block()
{
    node_t *stmnt_block = create_node(current_scope, NODE_STMNT_BLOCK);

    current_statement_block = stmnt_block;
    return stmnt_block;
}

node_t* handle_arg_block()
{
    node_t *arg_block = create_node(NULL, NODE_ARG_BLOCK);

    current_arg_block = arg_block;
    return arg_block;
}

