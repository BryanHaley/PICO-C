#ifndef AST_H
#define AST_H

#include <stdbool.h>

/* WHEN CREATING A NEW NODE TYPE:
 * Add its type to the node_type_e enum.
 * Declare its data struct below.
 * Write "create" method in tree_handler.c
 * Add a case to the switch in create_node in tree_handler.c
 * Write "generate" method in code_gen.c
 * Add a case to the switch in generate_node in code_gen.c
 * Don't forget to use breaks in above switch statements.
 */

typedef enum 
{
    NODE_GLOBAL_BLOCK,
    NODE_ARG_BLOCK,
    NODE_ARG_DEF,
    NODE_ARG_DEF_BLOCK,
    NODE_ARR_ACCESS,
    NODE_ARR_ACCESSOR,
    NODE_ARR_DEC,
    NODE_ARR_DIM,
    NODE_ARR_DIM_BLOCK,
    NODE_ARR_MULTI_ACCESSOR,
    NODE_ARR_MULTI_DIM_DEC,
    NODE_ASSIGN,
    NODE_ASSIGN_DEST,
    NODE_BIN_EXPR,
    NODE_DEC,
    NODE_DEC_W_ASSIGN,
    NODE_FUNC_CALL,
    NODE_FUNC_DEF,
    NODE_LIT_BLOCK,
    NODE_NULL_STMNT,
    NODE_OBJ_ACCESSOR_BLOCK,
    NODE_POSTFIX,
    NODE_PRI,
    NODE_PRIMARY,
    NODE_STMNT_BLOCK,
    NODE_STRUCT_DEF,
    NODE_STRUCT_INIT,
    NODE_STRUCT_MEM_BLOCK,
    NODE_SYMBOL
} node_type_e;

typedef struct node_t node_t;

struct node_t
{
    node_t *parent;
    node_type_e node_type;
    bool end_line;
    bool increase_indent;
    bool global;
    bool member;
    bool global_statement;
    
    void *data;
};

typedef enum
{
    PRI_LITERAL_NUM,
    PRI_LITERAL_STR,
    PRI_IDENTIFIER,
    PRI_FUNC_CALL,
    PRI_ARR_ACCESS
} primary_type_e;

typedef union
{
    double numValue;
    char* strValue;
    node_t* nodeValue;
} primary_value;

typedef struct 
{
    node_t *global_block;
} tree_t;

typedef struct 
{
    int num_children;
    node_t **children;
} parent_block_data;

typedef struct 
{
    char* return_type;
    char* identifier;
    node_t* arg_def_block;
    node_t* statement_block;
} func_def_data;

typedef struct 
{
    char* identifier;
    node_t* arg_block;
} func_call_data;

typedef struct 
{
    char* type;
    char* identifier;
} arg_def_data;

typedef struct 
{
    node_t* dest;
    node_t* expr;
    char* op;
} assign_data;

typedef struct 
{
    char* type;
    char* identifier;
} declaration_data;

typedef struct 
{
    char* type;
    char* identifier;
    node_t* expr;
} declaration_with_assign_data;

typedef struct
{
    node_t* left_node;
    node_t* right_node;
    char* op;
    
    bool in_parentheses;
    char* unary;
} bin_expr_data;

typedef struct
{
    primary_type_e val_type;
    primary_value val;

    bool in_parentheses;
    char* unary;
} primary_data;

typedef struct
{
    char* identifier;
    char* op;
} postfix_data;

typedef struct
{
    char* identifier;
    node_t* accessors;
} array_access_data;

typedef struct
{
    node_t* expr;
} array_accessor_data;

typedef struct
{
    char* identifier;
    int size;
    node_t* literal_block;
} array_dec_data;

typedef struct
{
    char* identifier;
    node_t* member_block;
} struct_def_data;

typedef struct
{
    char* type;
    char* identifier;
} struct_init_data;

typedef struct
{
    int num;
} array_dim_data;

typedef struct
{
    char* identifier;
    node_t* dimensions;
    node_t* literal_block;
} array_multidim_dec_data;

typedef struct
{
    char* identifier;
} symbol_data;

#endif
