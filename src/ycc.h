#define _GNU_SOURCE
#ifndef _YCC_H_
#define _YCC_H_
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ERROR(fmt, ...) \
    error(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

typedef struct Token {
    int type;        // type of token(integer)
    char *type_name; // type name(string)
    int value;       // value of integer token
    char *name;      // name of identifier token
    char *input;     // tokens which remain to be tokenized
} Token;

typedef struct Node {
    int node_type;                 // type of node(integer)
    struct C_type *c_type;         // type of C
    int value;                     // value of integer node
    char *name;                    // name of identifier node
    struct Node *lhs;              // left-hand side node
    struct Node *rhs;              // right-hand side node
    struct Node *condition;        // node of (if|while|for)-condition
    struct Node *then;             // node of if-then
    struct Node *els;              // node of if-else
    struct Node *init;             // node of for-loop variable
    struct Node *inc;              // for-loop variable update
    struct Vector *args;           // node of function arguments
    struct Vector *params;         // node of function params
    struct Map *lvar_offset_map;   // variables for function
    int max_variable_offset;       // sum of variable offset for function
    struct Node *body;             // compound statement
    struct Vector *stmts_in_block; // nodes of block scope
} Node;

typedef struct C_type {
    int type;
    int size;
    struct C_type *ptr_to;
    struct C_type *array_of;
    int array_len;  // How many elements
    int array_size; // size * array_len
} C_type;

typedef struct Vector {
    void **data;  // array of stored data
    int capacity; // maximum numbers of data to store
    int len;      // actual array_len of data array
} Vector;

typedef struct Map {
    Vector *keys;
    Vector *values;
} Map;

typedef struct StringBuilder {
    char *entity; // entity of string
    int capacity; // maximum array_len to store
    int len;      // actual array_len
} StringBuilder;

enum Token_kind {
    TK_NUM = 256, // Number literal
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
    TK_OR,        // ||
    TK_AND,       // &&
    TK_IDENT,     // Identifier
    TK_IF,        // if
    TK_ELSE,      // else
    TK_WHILE,     // while
    TK_FOR,       // for
    TK_RETURN,    // return
    TK_SIZEOF,    // sizeof
    TK_CHAR,      // char type
    TK_INT,       // int type
    TK_EOF,       // End Of Statement
};

enum Node_kind {
    ND_DEF_FUNC = 512, // define function
    ND_FUNCCALL,       // function call
    ND_RETURN,         // return
    ND_IF,             // if
    ND_WHILE,          // while
    ND_FOR,            // for
    ND_BLOCK,          // { }
    ND_ASSIGN,         // =
    ND_OR,             // ||
    ND_AND,            // &&
    ND_EQ,             // ==
    ND_NE,             // !=
    ND_LESS,           // <
    ND_LE,             // <=
    ND_ADD,            // +
    ND_SUB,            // -
    ND_MUL,            // *
    ND_DIV,            // /
    ND_NOT,            // !
    ND_ADDR,           // address-of operator
    ND_DEREF,          // dereference operator
    ND_SIZEOF,         // sizeof
    ND_NUM,            // Number literal
    ND_LVAR,           // Local variable
    ND_GVAR,           // Global variable
};

enum Type_kind {
    TY_CHAR,
    TY_INT,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
};

// ---tokenize.c---
Vector *tokenize(char *code);

// ---parse.c---
C_type *new_type(int, int, C_type *);
Vector *parse(Vector *v);
Vector *program();
Node *definition();
Node *define_func();
Vector *func_params();
Node *stmt();
Node *expr();
Node *assign();
Node *logical_or();
Node *logical_and();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *decl_var();

// ---sema_parse.c---
void sema_parse(Node *node);
void sema_parse_nodes(Vector *nodes);

// ---codegen.c---
void codegen(Vector *nodes);
void gen(Node *);

// ---util.c---
void error(const char *, const char *, int line, const char *fmt, ...);
Vector *new_vector();
void vec_push(Vector *, void *);
Map *new_map();
void map_set(Map *, char *, void *);
void *map_get(Map *, char *);
bool map_exists(Map *, char *);
StringBuilder *new_stringbiulder();
void stringbuilder_append(StringBuilder *, char *);

// ---util_test.c---
void util_test();

// ---dump.c---
void dump_token(Vector *);
void dump_nodes(Vector *);

#endif
