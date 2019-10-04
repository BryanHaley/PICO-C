#include <stdio.h>
#include <string.h>
#include "code_gen.h"
#include "tree_handler.h"
#include "ast.h"

void generate_code(FILE* out, tree_t* syntax_tree)
{
    output_file = out;
    
    generate_node(syntax_tree->global_block);
    fprintf(output_file, "\n");

    // Print utility functions at end of file for now
    FILE* util_functions = fopen("compiler_util_functions.lua", "r");

    int c = 1;

    while (c != EOF)
    {
        c = fgetc(util_functions);

        if (c == EOF) { break; }

        fprintf(output_file, "%c", (char) c);
    }
}

void generate_node(node_t* node)
{
    if (node == NULL) { return; }

    if      (node->global_statement) { indent_level = 0; }
    else if (node->increase_indent)  { indent_level++;   }
    
    if (node->end_line || node->member) for (int i = 0; i < indent_level; i++)
    {
        fprintf(output_file, INDENT_TOKEN);
    }

    switch(node->node_type)
    {
        case (NODE_FUNC_DEF):
            generate_func_def(node);
            break;
        case (NODE_FUNC_CALL):
            generate_func_call(node);
            break;
        case (NODE_ARG_DEF_BLOCK):
            generate_parent_block(node, ", ");
            break;
        case (NODE_ARG_DEF):
            generate_arg_def(node);
            break;
        case (NODE_ARG_BLOCK):
            generate_parent_block(node, ", ");
            break;
        case (NODE_LIT_BLOCK):
            generate_parent_block(node, ", ");
            break;
        case (NODE_STRUCT_MEM_BLOCK):
            generate_parent_block(node, ",\n");
            break;
        case (NODE_OBJ_ACCESSOR_BLOCK):
            generate_parent_block(node, ".");
            break;
        case (NODE_BIN_EXPR):
            generate_bin_expr(node);
            break;
        case (NODE_PRI):
            generate_primary(node);
            break;
        case (NODE_NULL_STMNT):
            //fprintf(output_file, ";");
            break;
        case (NODE_ASSIGN):
            generate_assignment(node);
            break;
        case (NODE_DEC):
            generate_declaration(node);
            break;
        case (NODE_DEC_W_ASSIGN):
            generate_declaration_with_assign(node);
            break;
        case (NODE_POSTFIX):
            generate_postfix(node);
            break;
        case (NODE_ARR_ACCESS):
            generate_array_access(node);
            break;
        case (NODE_ARR_ACCESSOR):
            generate_array_accessor(node);
            break;
        case (NODE_ARR_DEC):
            generate_array_declaration(node);
            break;
        case (NODE_ARR_DIM):
            generate_array_dim(node);
            break;
        case (NODE_ARR_MULTI_DIM_DEC):
            generate_mutli_dim_array_dec(node);
            break;
        case (NODE_STRUCT_DEF):
            generate_struct_declaration(node);
            break;
        case (NODE_STRUCT_INIT):
            generate_struct_initialization(node);
            break;
        case (NODE_SYMBOL):
            generate_symbol(node);
            break;
        default:
            generate_parent_block(node, NULL);
            break;
    }

    if ((node->end_line || node->member) && node->increase_indent && indent_level > 0)
    { indent_level--; }

    if (node->end_line)
    { fprintf(output_file, "\n"); }
}

void generate_parent_block(node_t* parent, char* delim)
{
    if (parent == NULL) { return; }

    parent_block_data* parent_data = (parent_block_data*) parent->data;

    for (int i = 0; i < parent_data->num_children; i++)
    {
        generate_node(parent_data->children[i]);

        if (delim != NULL && i != parent_data->num_children-1)
        {
            fprintf(output_file, "%s", delim);
        }
    }
}

void generate_func_def(node_t* node)
{
    if (node == NULL) { return; }

    func_def_data* data = (func_def_data*) node->data;

    fprintf(output_file, "function %s (", data->identifier);

    if (data->arg_def_block != NULL)
    { 
        generate_node(data->arg_def_block);
    }

    fprintf(output_file, ")\n");
    
    if (data->statement_block != NULL)
    { 
        generate_node(data->statement_block); 
    }

    fprintf(output_file, "end\n");
}

void generate_arg_def(node_t* node)
{
    if (node == NULL) { return; }
    
    arg_def_data* data = (arg_def_data*) node->data;

    fprintf(output_file, "%s", data->identifier);
}

void generate_func_call(node_t* node)
{
    if (node == NULL) { return; }
    
    func_call_data* data = (func_call_data*) node->data;

    fprintf(output_file, "%s(", data->identifier);

    if (data->arg_block != NULL)
    {
        generate_node(data->arg_block);
    }

    fprintf(output_file, ")");
}

void generate_bin_expr(node_t* node)
{
    if (node == NULL) { return; }
    
    bin_expr_data* data = (bin_expr_data*) node->data;

    if (data->unary != NULL) { fprintf(output_file, "%s", data->unary); }

    if (data->in_parentheses) { fprintf(output_file, "("); }

    generate_node(data->left_node);
    fprintf(output_file, " %s ", data->op);
    generate_node(data->right_node);
    
    if (data->in_parentheses) { fprintf(output_file, ")"); }
}

void generate_primary(node_t* node)
{
    if (node == NULL) { return; }
    
    primary_data* data = (primary_data*) node->data;

    if (data->unary != NULL) { fprintf(output_file, "%s", data->unary); }

    if (data->in_parentheses) { fprintf(output_file, "("); }

    switch (data->val_type)
    {
        case (PRI_LITERAL_NUM):
            fprintf(output_file, "%g", data->val.numValue);
            break;
        case (PRI_LITERAL_STR):
        case (PRI_IDENTIFIER):
            fprintf(output_file, "%s", data->val.strValue);
            break;
        default:
            generate_node(data->val.nodeValue);
            break;
    }

    if (data->in_parentheses) { fprintf(output_file, ")"); }
}

void generate_assignment(node_t* node)
{
    if (node == NULL) { return; }

    assign_data* data = (assign_data*) node->data;

    generate_node(data->dest);

    fprintf(output_file, " %s ", data->op);

    generate_node(data->expr);
}

void generate_declaration(node_t* node)
{
    if (node == NULL) { return; }

    declaration_data* data = (declaration_data*) node->data;

    if (strcmp(data->type, "array") == 0)
    {
        /* error, array must be declared with [] */
        return;
    }

    if (!node->member)
    {
        if (node->global) { fprintf(output_file, "global "); }
        else              { fprintf(output_file, "local ");  }
    }

    fprintf(output_file, "%s", data->identifier);

    if (strcmp(data->type, "var") == 0)
    { fprintf(output_file, " = 0"); }
    else if (strcmp(data->type, "string") == 0)
    { fprintf(output_file, " = \"\""); }
    else if (strcmp(data->type, "bool") == 0)
    { fprintf(output_file, " = false"); }
    else
    { fprintf(output_file, " = nil"); }
}

void generate_declaration_with_assign(node_t* node)
{
    if (node == NULL) { return; }

    declaration_with_assign_data* data = (declaration_with_assign_data*) node->data;

    if (!node->member)
    {
        if (node->global) { fprintf(output_file, "global "); }
        else              { fprintf(output_file, "local ");  }
    }

    fprintf(output_file, "%s = ", data->identifier);

    generate_node(data->expr);
}

void generate_postfix(node_t* node)
{
    if (node == NULL) { return; }

    postfix_data* data = (postfix_data*) node->data;

    fprintf(output_file, "%s%s", data->identifier, data->op);
}

void generate_array_access(node_t* node)
{
    if (node == NULL) { return; }

    array_access_data* data = (array_access_data*) node->data;

    fprintf(output_file, "%s", data->identifier);

    /* TODO: Figure out if I only want to allow 'sane' indices or not.
     *       i.e. only allow indexes of 0 to n, as opposed to an associative array.
     *       Might separate those into arrays and tables, respectively.
     */

    generate_node(data->accessors);
}

void generate_array_accessor(node_t* node)
{
    if (node == NULL) { return; }

    array_accessor_data* data = (array_accessor_data*) node->data;

    fprintf(output_file, "[");
    generate_node(data->expr);
    fprintf(output_file, "]");
}

void generate_array_declaration(node_t* node)
{
    if (node == NULL) { return; }

    array_dec_data* data = (array_dec_data*) node->data;

    parent_block_data* litbl_data = NULL;
    bool has_literal_block = false;
    int literal_block_children = 0;

    if (data->literal_block != NULL)
    {
        litbl_data = (parent_block_data*) (data->literal_block->data);
        literal_block_children = litbl_data->num_children;

        if (literal_block_children > 0)
        {
            has_literal_block = true;
        }
    }

    if (node->global) { fprintf(output_file, "global "); }
    else              { fprintf(output_file, "local ");  }

    fprintf(output_file, "%s", data->identifier);

    if (data->size > 0 && has_literal_block)
    {
        if (literal_block_children != 1 && literal_block_children != data->size)
        {
            // error, array initializer must has 1 or n elements
            return;
        }
    }

    else if (data->size > 0 && !has_literal_block)
    {
        // error, non-empty arrays must have initializer with 1 or n elements
        return;
    }

    fprintf(output_file, " = { ");

    if (has_literal_block)
    {
        if (data->size == 0 || data->size == literal_block_children)
        {
            generate_node(data->literal_block);
            fprintf(output_file, " }");
        }

        else if (data->size > 1 && literal_block_children == 1)
        {
            fprintf(output_file, " }\n");

            char* loop_index = "_PICO-C_ARR_I"; // TODO: make sure this isn't double declared
            fprintf(output_file, "%sfor %s=%d, %d do\n",
                    INDENT_TOKEN, loop_index, 0, data->size);

            fprintf(output_file, "%s%s%s[%s] = ",
                    INDENT_TOKEN, INDENT_TOKEN, data->identifier, loop_index);

            generate_node(litbl_data->children[0]);
            
            fprintf(output_file, "\n%send",
                    INDENT_TOKEN);
        }
    }

    else
    {
        fprintf(output_file, "}");
    }
}

void generate_struct_declaration(node_t* node)
{
    if (node == NULL) { return; }

    struct_def_data* data = (struct_def_data*) node->data;

    fprintf(output_file, "%s = readonlytable\n{\n", data->identifier);

    generate_node(data->member_block);

    fprintf(output_file, "\n}\n");
}

void generate_struct_initialization(node_t* node)
{
    if (node == NULL) { return; }

    struct_init_data* data = (struct_init_data*) node->data;

    // TODO: don't hard-code compiler utility methods
    fprintf(output_file, "%s = _PCC_SHALLOW_COPY(%s)", data->identifier, data->type);
}

void generate_array_dim(node_t* node)
{
    if (node == NULL) { return; }

    array_dim_data* data = (array_dim_data*) node->data;

    fprintf(output_file, "[%d]", data->num);
}

void generate_mutli_dim_array_dec(node_t* node)
{
    if (node == NULL) { return; }

    array_multidim_dec_data* data = (array_multidim_dec_data*) node->data;

    parent_block_data* dimensions_data = (parent_block_data*) (data->dimensions->data);
    int num_dimensions = dimensions_data->num_children;

    if (node->global) { fprintf(output_file, "global "); }
    else              { fprintf(output_file, "local ");  }

    // TODO: don't hard-code compiler utility methods
    fprintf(output_file, "%s = _PCC_NEW_MULTI_DIM_ARRAY(%d)", data->identifier, num_dimensions);

    /* TODO: generate for loops for setting initial value */
}

void generate_symbol(node_t* node)
{
    if (node == NULL) { return; }

    symbol_data* data = (symbol_data*) node->data;

    fprintf(output_file, "%s", data->identifier);
}