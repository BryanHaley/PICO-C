#include <stdlib.h>
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

parent_block_data* add_child_to_parent_block(node_t* parent, int num_new_children, node_t** children)
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

		parent_data->num_children = num_new_children;
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

symbol_t* get_var_symbol(char* name, double value)
{
	symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
	symbol->type = TYPE_VAR;
	symbol->name = name;
	symbol->value = value;

	return symbol;
}

symbol_t* get_pointer_symbol(char* name)
{
	symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
	symbol->type = TYPE_POINTER;
	symbol->name = name;

	return symbol;
}

symbol_t* get_literal_num_symbol(double value)
{
	symbol_t *symbol = (symbol_t*) calloc(1, sizeof(symbol_t));
	symbol->type = TYPE_LITERAL_NUM;
	symbol->value = value;

	return symbol;
}

void handle_global_block()
{
	syntax_tree = (tree_t*) calloc(1, sizeof(tree_t));
	
	node_t *global_block = create_node(NULL, NODE_GLOBAL_BLOCK);

	syntax_tree->global_block = global_block;

	current_scope = global_block;
}

void handle_function_def(node_t* arg_def_block, node_t* statement_block)
{
	if (current_scope != syntax_tree->global_block)
	{
		yyerror("Functions can only be defined in the global scope");
	}

	node_t *func_def_node = create_node(syntax_tree->global_block, NODE_FUNC_DEF);

	node_t* new_children[2] = { arg_def_block, statement_block };
	add_child_to_parent_block(func_def_node, 2, new_children);

	node_t* new_gb_children[1] = { func_def_node };
	add_child_to_parent_block(syntax_tree->global_block, 1, new_gb_children);

	current_scope = func_def_node;
}

void handle_func_call(symbol_t* func, node_t* arg_block)
{
	if (current_scope == syntax_tree->global_block)
	{
		yyerror("Functions cannot be called in the global scope");
	}

	node_t *func_call = create_node(current_scope, NODE_FUNC_CALL);
	func_call_data *data = (func_call_data*) func_call->data;

	data->identifier = func;
	data->argument_block = arg_block;
}

void handle_arg_def(data_types_e type, char* symbol_name, double symbol_value)
{
	node_t *arg_def_node = create_node(current_parent_node, NODE_ARG_DEF);
	arg_def_data *arg_def_node_data = (arg_def_data*) arg_def_node->data;

	switch (type)
	{
		case (TYPE_VAR):
			arg_def_node_data->identifier = get_var_symbol(symbol_name, symbol_value);
			break;
		// Implement others
	}
	
	node_t* child[1] = { arg_def_node };
	add_child_to_parent_block(current_parent_node, 1, child);
}

node_t* handle_arg_def_block()
{
	node_t *arg_def_block = create_node(NULL, NODE_ARG_DEF_BLOCK);

	current_parent_node = arg_def_block;
	return arg_def_block;
}

node_t* handle_statement_block()
{
	node_t *stmnt_block = create_node(current_scope, NODE_STMNT_BLOCK);

	current_parent_node = stmnt_block;
	return stmnt_block;
}

node_t* handle_arg_block()
{
	node_t *arg_block = create_node(NULL, NODE_ARG_BLOCK);

	current_parent_node = arg_block;
	return arg_block;
}