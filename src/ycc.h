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
#define ERROR(fmt, ...)                                                        \
    error(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

typedef struct Token {
    int type;        // type of token(integer)
    char *type_name; // type name(string)
    int value;       // value of integer token
    char *name;      // name of identifier token
    char *input;     // tokens which remain to be tokenized
} Token;

typedef struct Node {
    int type;                      // type of node(integer)
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
    struct Map *vars;              // variables for function
    int max_variable_offset;       // sum of variable offset
    struct Node *body;             // compound statement
    struct Vector *stmts_in_block; // nodes of block scope
} Node;

typedef struct Vector {
    void **data;  // array of stored data
    int capacity; // maximum numbers of data to store
    int len;      // actual length of data array
} Vector;

typedef struct Map {
    Vector *keys;
    Vector *values;
} Map;

typedef struct StringBuilder {
    char *entity; // entity of string
    int capacity; // maximum length to store
    int len;      // actual length
} StringBuilder;

enum Token_type {
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
    TK_INT,       // int type
    TK_EOF,       // End Of Statement
};

enum Node_type {
    ND_NUM = 512, // Number literal
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LE,        // <=
    ND_GE,        // >=
    ND_OR,        // ||
    ND_AND,       // &&
    ND_IDENT,     // Identifier
    ND_IF,        // if
    ND_WHILE,     // while
    ND_FOR,       // for
    ND_ADDR,      // address-of operator
    ND_DEREF,     // dereference operator
    ND_FUNCCALL,  // function call
    ND_DEF_FUNC,  // define function
    ND_RETURN,    // return
    ND_BLOCK,     // { }
};

// ---tokenize.c---
Vector *tokenize(char *code);

// ---parse.c---
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
