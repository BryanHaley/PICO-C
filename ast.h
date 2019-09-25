#ifndef AST_H
#define AST_H

#include <stdbool.h>

typedef enum 
{
    NODE_GLOBAL_BLOCK,
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_ARG_DEF,
    NODE_ASSIGN,
    NODE_DEC,
    NODE_DEC_W_ASSIGN,
    NODE_NULL_STMNT,
    NODE_STMNT_BLOCK,
    NODE_ARG_DEF_BLOCK,
    NODE_ARG_BLOCK,
    NODE_PRI,
    NODE_BIN_EXPR,
    NODE_PRIMARY
} node_type_e;

typedef struct node_t node_t;

struct node_t
{
    node_t *parent;
    node_type_e node_type;
    
    void *data;
};

typedef enum
{
    PRI_LITERAL_NUM,
    PRI_LITERAL_STR,
    PRI_IDENTIFIER,
    PRI_FUNC_CALL
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
    char* identifier;
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
} bin_expr_data;

typedef struct
{
    primary_type_e val_type;
    primary_value val;
} primary_data;

#endif
