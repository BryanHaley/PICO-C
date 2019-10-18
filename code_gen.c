#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_gen.h"
#include "tree_handler.h"
#include "pcc_grammar.h"
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

void code_gen_error(int line_no, char* err)
{
    fprintf(stderr, "\nERROR in line %d: %s\n", line_no, err);
    err_in_code_gen = true;
    //remove(output_file);
    return;
}

char* get_unique_name()
{
    return get_unique_name_with_prefix("_PCC_VAR_");
}

char* get_unique_name_with_prefix(char* prefix)
{
    unique_name_suffix_num++;

    // see for buffer size formula: https://stackoverflow.com/a/10536254
    size_t buff_size = strlen(prefix)+(3*sizeof(int)+2);
    char* str_buff = (char*) calloc(buff_size, sizeof(char));

    size_t length = snprintf(str_buff, buff_size, "%s%d", prefix, unique_name_suffix_num);

    str_buff = realloc((void*) str_buff, length+1);

    return str_buff;
}

void print_indents_with_additional(int additional)
{
    for (int i = 0; i < indent_level+additional; i++)
    {
        fprintf(output_file, INDENT_TOKEN);
    }
}

void print_indents()
{
    print_indents_with_additional(0);
}

void generate_node(node_t* node)
{
    if (err_in_code_gen) { return; }
    if (node == NULL)    { return; }

    if      (node->global_statement) { indent_level = 0; }
    else if (node->increase_indent)  { indent_level++;   }
    
    if (node->end_line || node->member) 
    { print_indents(); }

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
        case (NODE_CASE_BLOCK):
            generate_parent_block(node, ",\n");
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
            generate_struct_definition(node);
            break;
        case (NODE_STRUCT_INIT):
            generate_struct_initialization(node);
            break;
        case (NODE_SYMBOL):
            generate_symbol(node);
            break;
        case (NODE_METHOD_CALL):
            generate_method_call(node);
            break;
        case (NODE_IF_STMNT):
            generate_if_stmnt(node);
            break;
        case (NODE_ELSEIF_STMNT):
            generate_elseif_stmnt(node);
            break;
        case (NODE_ELSE_STMNT):
            generate_else_stmnt(node);
            break;
        case (NODE_FOR_LOOP):
            generate_for_loop(node);
            break;
        case (NODE_WHILE_LOOP):
            generate_while_loop(node);
            break;
        case (NODE_DO_WHILE_LOOP):
            generate_do_while_loop(node);
            break;
        case (NODE_DO_UNTIL_LOOP):
            generate_do_until_loop(node);
            break;
        case (NODE_LABELMAKER):
            generate_labelmaker(node);
            break;
        case (NODE_GOTO):
            generate_goto_statement(node);
            break;
        case (NODE_BREAK):
            generate_break_statement(node);
            break;
        case (NODE_RETURN):
            generate_return_statement(node);
            break;
        case (NODE_CONTINUE):
            generate_continue_statement(node);
            break;
        case (NODE_SWITCH):
            generate_switch_statement(node);
            break;
        case (NODE_FAST_SWITCH):
            generate_fast_switch_statement(node);
            break;
        case (NODE_CASE):
            generate_case(node);
            break;
        case (NODE_FSWITCH_CALL):
            generate_fswitch_call(node);
            break;
        case (NODE_NULL):
            generate_null(node);
            break;
        default:
            generate_parent_block(node, NULL);
            break;
    }

    if (node->increase_indent && indent_level > 0)
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
            fprintf(output_file, "%s", data->val.strValue);
            break;
        case (PRI_LITERAL_BOOL):
            fprintf(output_file, "%s", data->val.boolValue ? "true" : "false");
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
        code_gen_error(node->line_no, "Array must be declared using brackets [].");
        return;
    }

    if (!node->member && !node->dont_specify_global)
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

    if (!node->member && !node->dont_specify_global)
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

    if (!node->dont_specify_global)
    {
        if (node->global) { fprintf(output_file, "global "); }
        else              { fprintf(output_file, "local ");  }
    }

    fprintf(output_file, "%s", data->identifier);

    if (data->size > 0 && has_literal_block)
    {
        if (literal_block_children != 1 && literal_block_children != data->size)
        {
            code_gen_error(node->line_no, "Array initializer must have 1 or n elements.");
            return;
        }
    }

    else if (data->size > 0 && !has_literal_block)
    {
        code_gen_error(node->line_no, "Non-empty arrays must have initializer with 1 or n elements.");
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

void generate_struct_definition(node_t* node)
{
    if (node == NULL) { return; }

    struct_def_data* data = (struct_def_data*) node->data;

    fprintf(output_file, "%s = {\n", data->identifier);

    generate_node(data->member_block);

    fprintf(output_file, "\n}\n");
}

void generate_struct_initialization(node_t* node)
{
    if (node == NULL) { return; }

    struct_init_data* data = (struct_init_data*) node->data;

    // TODO: don't hard-code compiler utility methods
    fprintf(output_file, "_PCC_SHALLOW_COPY(%s)", data->type);
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

    if (!node->dont_specify_global)
    {
        if (node->global) { fprintf(output_file, "global "); }
        else              { fprintf(output_file, "local ");  }
    }

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

void generate_method_call(node_t* node)
{
    if (node == NULL) { return; }

    method_call_data* data = (method_call_data*) node->data;

    generate_node(data->obj_access);
    fprintf(output_file, ".");
    generate_node(data->func_call);
}

void generate_if_stmnt(node_t* node)
{
    if (node == NULL) { return; }

    if_stmnt_data* data = (if_stmnt_data*) node->data;

    fprintf(output_file, "if ");
    generate_node(data->rel_expr);
    fprintf(output_file, " then\n");
    generate_node(data->stmnt_block);

    if (data->elseif_block != NULL) { generate_node(data->elseif_block); }
    if (data->else_stmnt != NULL)   { generate_node(data->else_stmnt); }
    
    print_indents();
    fprintf(output_file, "end");
}

void generate_elseif_stmnt(node_t* node)
{
    if (node == NULL) { return; }

    elseif_stmnt_data* data = (elseif_stmnt_data*) node->data;

    print_indents();
    fprintf(output_file, "elseif ");
    generate_node(data->rel_expr);
    fprintf(output_file, " then\n");
    generate_node(data->stmnt_block);
}

void generate_else_stmnt(node_t* node)
{
    if (node == NULL) { return; }

    else_stmnt_data* data = (else_stmnt_data*) node->data;

    print_indents();
    fprintf(output_file, "else\n");
    generate_node(data->stmnt_block);
}

void generate_for_loop(node_t* node)
{
    if (node == NULL) { return; }

    for_loop_data* data = (for_loop_data*) node->data;

    data->assign_stmnt->end_line = false;
    data->assign_stmnt->increase_indent = false;
    data->rel_expr->end_line = false;
    data->rel_expr->increase_indent = false;
    data->inc_stmnt->end_line = false;
    data->inc_stmnt->increase_indent = false;

    // check for correct types

    if (data->assign_stmnt->node_type != NODE_DEC_W_ASSIGN &&
        data->assign_stmnt->node_type != NODE_ASSIGN)
    {
        code_gen_error(node->line_no, "First statement in for loop must be an assignment.");
    }

    if (data->inc_stmnt->node_type == NODE_DEC_W_ASSIGN)
    {
        code_gen_error(node->line_no, "Last statement in for loop cannot declare a variable.");
    }

    else if (data->inc_stmnt->node_type != NODE_ASSIGN && data->inc_stmnt->node_type != NODE_POSTFIX)
    {
        code_gen_error(node->line_no, "Last statement in for loop must be an assignment or postfix statement.");
    }

    /* Complex for loops may need to implemented as while loops
     * I define a complex for loop as a for loop where increment
     * statement is not a simple ++, --, +=, or -=, or where
     * the relational expression does not take the form of
     * i < n.
     */
    bool implement_as_while;

    if (data->inc_stmnt->node_type == NODE_ASSIGN)
    {
        assign_data* inc_stmnt_data = (assign_data*) data->inc_stmnt->data;
        char* assign_op = inc_stmnt_data->op;

        if (strcmp(assign_op, "+=") != 0 && strcmp(assign_op, "-=") != 0)
        {
            implement_as_while = true;
        }
    }

    /* implement as for loop */
    if (!implement_as_while)
    {
        bin_expr_data* rel_expr_data = (bin_expr_data*) data->rel_expr->data;
        data->assign_stmnt->dont_specify_global = true;

        fprintf(output_file, "for ");
        generate_node(data->assign_stmnt);
        fprintf(output_file, ", ");
        generate_node(rel_expr_data->right_node);

        if (data->inc_stmnt->node_type == NODE_POSTFIX)
        {
            postfix_data* inc_stmnt_data = (postfix_data*) data->inc_stmnt->data;

            if (strcmp(inc_stmnt_data->op, "--") == 0)
            {
                fprintf(output_file, ", -1");
            }

            // Redundant in Lua. Save the tokens.
            /*else if (strcmp(inc_stmnt_data->op, "++") == 0)
            {
                fprintf(output_file, "1");
            }*/

            else if (strcmp(inc_stmnt_data->op, "++") != 0)
            {
                code_gen_error(node->line_no, "Invalid postfix op used in for loop incrementor.");
                return;
            }
        }

        else if (data->inc_stmnt->node_type == NODE_ASSIGN)
        {
            assign_data* inc_stmnt_data = (assign_data*) data->inc_stmnt->data;

            if (strcmp(inc_stmnt_data->op, "+=") == 0)
            {
                fprintf(output_file, ", ");
                generate_node(inc_stmnt_data->expr);
            }

            else if (strcmp(inc_stmnt_data->op, "-=") == 0)
            {
                fprintf(output_file, ", -(");
                generate_node(inc_stmnt_data->expr);
                fprintf(output_file, ")");
            }

            else
            {
                code_gen_error(node->line_no, "Invalid assignment op used in for loop incrementor.");
                return;
            }
        }

        fprintf(output_file, " do\n");
        generate_node(data->stmnt_block);
        
        // see check_continue_statement in tree_checker.c
        if (data->continue_statement != NULL)
        {
            continue_statement_data* continue_data = (continue_statement_data*) data->continue_statement->data;
            print_indents();
            fprintf(output_file, "-- PCC: continue statement label --\n");
            print_indents_with_additional(1);
            fprintf(output_file, "::%s::\n", continue_data->identifier);
        }

        print_indents();
        fprintf(output_file, "end");
    }

    /* implement as while loop */
    else
    {
        data->inc_stmnt->end_line = true;
        data->inc_stmnt->increase_indent = true;

        if (data->assign_stmnt->node_type == NODE_DEC_W_ASSIGN)
        {
            /* Note: this isn't going to work properly until symbols are properly
             * implemented. */
            /*data->assign_stmnt->global = false;
            data->assign_stmnt->dont_specify_global = false;

            declaration_with_assign_data* assign_stmnt_data =
                (declaration_with_assign_data*) data->assign_stmnt->data;

            char* unique_identifier_suffix = get_unique_identifier_suffix();
            
            strcat(assign_stmnt_data->identifier, unique_identifier_suffix);*/

            generate_node(data->assign_stmnt);
        }
        
        else
        {
            generate_node(data->assign_stmnt);
        }

        fprintf(output_file, "\n");
        print_indents();
        fprintf(output_file, "while ");
        generate_node(data->rel_expr);
        fprintf(output_file, " do\n");

        generate_node(data->stmnt_block);

        // see check_continue_statement in tree_checker.c
        if (data->continue_statement != NULL)
        {
            continue_statement_data* continue_data = (continue_statement_data*) data->continue_statement->data;
            print_indents();
            fprintf(output_file, "-- PCC: continue statement label --\n");
            print_indents_with_additional(1);
            fprintf(output_file, "::%s::\n", continue_data->identifier);
        }

        print_indents();
        fprintf(output_file, "-- PCC: increment for loop --\n");
        generate_node(data->inc_stmnt);
        print_indents();
        fprintf(output_file, "end");
    }
}

void generate_while_loop(node_t* node)
{
    if (node == NULL) { return; }

    while_loop_data* data = (while_loop_data*) node->data;

    data->rel_expr->end_line = false;
    data->rel_expr->increase_indent = false;

    // see check_continue_statement in tree_checker.c
    if (data->continue_statement != NULL)
    {
        continue_statement_data* continue_data = (continue_statement_data*) data->continue_statement->data;
        fprintf(output_file, "-- PCC: continue statement label --\n");
        print_indents();
        fprintf(output_file, "::%s::\n", continue_data->identifier);
        print_indents();
    }

    fprintf(output_file, "while ");
    generate_node(data->rel_expr);
    fprintf(output_file, " do\n");

    generate_node(data->stmnt_block);

    print_indents();
    fprintf(output_file, "end");
}

void generate_do_while_loop(node_t* node)
{
    if (node == NULL) { return; }

    do_while_loop_data* data = (do_while_loop_data*) node->data;

    data->rel_expr->end_line = false;
    data->rel_expr->increase_indent = false;

    fprintf(output_file, "repeat\n");

    generate_node(data->stmnt_block);

    // see check_continue_statement in tree_checker.c
    if (data->continue_statement != NULL)
    {
        continue_statement_data* continue_data = (continue_statement_data*) data->continue_statement->data;
        print_indents();
        fprintf(output_file, "-- PCC: continue statement label --\n");
        print_indents_with_additional(1);
        fprintf(output_file, "::%s::\n", continue_data->identifier);
    }

    print_indents();
    fprintf(output_file, "until not (");
    generate_node(data->rel_expr);
    fprintf(output_file, ")");
}

void generate_do_until_loop(node_t* node)
{
    if (node == NULL) { return; }

    do_until_loop_data* data = (do_until_loop_data*) node->data;

    data->rel_expr->end_line = false;
    data->rel_expr->increase_indent = false;

    fprintf(output_file, "repeat\n");

    generate_node(data->stmnt_block);

    // see check_continue_statement in tree_checker.c
    if (data->continue_statement != NULL)
    {
        continue_statement_data* continue_data = (continue_statement_data*) data->continue_statement->data;
        print_indents();
        fprintf(output_file, "-- PCC: continue statement label --\n");
        print_indents_with_additional(1);
        fprintf(output_file, "::%s::\n", continue_data->identifier);
    }

    print_indents();
    fprintf(output_file, "until ");
    generate_node(data->rel_expr);
}

void generate_labelmaker(node_t* node)
{
    if (node == NULL) { return; }

    labelmaker_data* data = (labelmaker_data*) node->data;

    fprintf(output_file, "::%s::", data->identifier);
}

void generate_goto_statement(node_t* node)
{
    if (node == NULL) { return; }

    goto_statement_data* data = (goto_statement_data*) node->data;

    fprintf(output_file, "goto %s", data->identifier);
}

void generate_break_statement(node_t* node)
{
    if (node == NULL) { return; }

    fprintf(output_file, "break");
}

void generate_return_statement(node_t* node)
{
    if (node == NULL) { return; }

    return_statement_data* data = (return_statement_data*) node->data;

    fprintf(output_file, "return ");

    if (data->expr != NULL) { generate_node(data->expr); }
}

void generate_continue_statement(node_t* node)
{
    if (node == NULL) { return; }

    continue_statement_data* data = (continue_statement_data*) node->data;

    fprintf(output_file, "goto %s", data->identifier);
}

void generate_switch_statement(node_t* node)
{
    if (node == NULL) { return; }

    switch_statement_data* data = (switch_statement_data*) node->data;

    // Unforuntately all "fast" methods of creating switch statements in Lua
    // rely on nested functions, which break the expected lexical scoping.
    // Breaking lexical scoping also means we can't use goto statements.
    // C-style switch statements have to be implemented the slow way.

    fprintf(output_file, "-- PCC: emulated switch-case --\n");
    generate_node(data->break_me);
    if (data->has_default) { generate_node(data->default_bool); }

    parent_block_data* case_block_data = (parent_block_data*) data->case_block->data;
    declaration_data* break_me_data = (declaration_data*) data->break_me->data;
    declaration_data* default_bool_data = (declaration_data*) data->default_bool->data;

    for (int i = 0; i < case_block_data->num_children; i++)
    {
        case_data* current_case_data = (case_data*) case_block_data->children[i]->data;

        if (current_case_data->expr != NULL)
        {
            fprintf(output_file, "\n");
            print_indents();
            fprintf(output_file, "if ((%s) or (", break_me_data->identifier);
            generate_node(data->expr);
            fprintf(output_file, " == ");
            generate_node(current_case_data->expr);
            fprintf(output_file, ")) then\n");

            print_indents_with_additional(1);
            fprintf(output_file, "%s = true\n", break_me_data->identifier);
            print_indents_with_additional(1);
            fprintf(output_file, "%s = false\n", default_bool_data->identifier);

            generate_node(current_case_data->stmnt_block);
            
            print_indents();
            fprintf(output_file, "end");
        }

        // Default case
        else
        {
            fprintf(output_file, "\n");
            print_indents();
            fprintf(output_file, "if (%s or %s) then\n", break_me_data->identifier, default_bool_data->identifier);

            generate_node(current_case_data->stmnt_block);
            
            print_indents();
            fprintf(output_file, "end");
        }
    }

    // NOTE: I've implemented a "fast switch" that works more like the Lua way, and can
    // be implemented as a table of functions. See below.
}

void generate_fast_switch_statement(node_t* node)
{
    if (node == NULL) { return; }

    fast_switch_data* data = (fast_switch_data*) node->data;

    fprintf(output_file, "%s = {\n", data->identifier);

    generate_node(data->case_block);

    fprintf(output_file, "\n");
    print_indents();
    fprintf(output_file, "}");
}

// used for fast switches only
void generate_case(node_t* node)
{
    if (node == NULL) { return; }

    case_data* data = (case_data*) node->data;
    fast_switch_data* fswitch_data = (fast_switch_data*) node->parent->parent->data;

    print_indents();
    fprintf(output_file, "[");

    if (data->expr != NULL)
    { generate_node(data->expr); }
    else
    { fprintf(output_file, "\"_PCC_DEFAULT\""); }

    fprintf(output_file, "] = function(");

    if (fswitch_data->params != NULL)
    { generate_node(fswitch_data->params); }
    
    fprintf(output_file, ")\n");
    generate_node(data->stmnt_block);
    print_indents();
    fprintf(output_file, "end");

    return;
}

void generate_fswitch_call(node_t* node)
{
    if (node == NULL) { return; }
    
    fswitch_call_data* data = (fswitch_call_data*) node->data;

    // TODO: Implement this in tree_checker.c
    bool fswitch_has_default = true;

    if (!fswitch_has_default)
    {
        fprintf(output_file, "%s[", data->identifier);
        if (data->expr != NULL) { generate_node(data->expr); }
        fprintf(output_file, "](");
        if (data->arg_block != NULL) { generate_node(data->arg_block); }
        fprintf(output_file, ")");
    }

    else
    {
        fprintf(output_file, "if %s[", data->identifier);
        if (data->expr != NULL) { generate_node(data->expr); }
        fprintf(output_file, "] then ");

        fprintf(output_file, "%s[", data->identifier);
        if (data->expr != NULL) { generate_node(data->expr); }
        fprintf(output_file, "](");
        if (data->arg_block != NULL) { generate_node(data->arg_block); }

        fprintf(output_file, "else %s[__PCC_DEFAULT](", data->identifier);
        if (data->arg_block != NULL) { generate_node(data->arg_block); }
        fprintf(output_file, ") end");
    }
}

void generate_null(node_t* node)
{
    fprintf(output_file, "nil");
}