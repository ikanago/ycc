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
    struct Node *condition;        // node of (if|while)-condition
    struct Node *then;             // node of if-then
    struct Node *els;              // node of if-else
    struct Vector *args;           // node of function arguments
    struct Vector *params;         // node of function params
    struct Map *vars;              // variables for function
    int max_variable_offset;       // sum of variable offset
    struct Node *body;             // compound statement
    struct Vector *stmts_in_block; // nodes of block scope
} Node;

typedef struct Vector {
    void **data;
    int capacity;
    int len;
} Vector;

typedef struct Map {
    Vector *keys;
    Vector *values;
} Map;

enum Token_type {
    TK_NUM = 256, // Number literal
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
    TK_IDENT,     // Ifentifier
    TK_IF,        // if
    TK_ELSE,      // else
    TK_WHILE,     // while
    TK_RETURN,    // return
    TK_EOF,       // End Of Statement
};

enum Node_type {
    ND_NUM = 512, // Number literal
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LE,        // <=
    ND_GE,        // >=
    ND_IDENT,     // Ifentifier
    ND_IF,        // if
    ND_WHILE,     // while
    ND_FUNCCALL,  // function call
    ND_DEF_FUNC,  // define function
    ND_RETURN,    // return
    ND_BLOCK,     // { }
};

// ---main.c---
extern Map *variable_map;
extern int variable_offset;

// ---tokenize.c---
Token *add_token(Vector *, int, char *);
bool is_alnum(char c);
Map *reserve_keywords();
Vector *tokenize(char *);
Vector *scan(char *);

// ---parse.c---
extern int variable_offset;
Node *new_node(int type, Node *lhs, Node *rhs);
Node *new_node_num(int value);
Node *new_node_var(char *name);
Node *new_node_funccall(char *name, Vector *args);
Node *new_node_var(char *name);
void expect(int type);
int consume(int type);
Vector *parse(Vector *v);
Vector *program();
Node *definition();
Node *define_func();
Vector *func_params();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

// ---codegen.c---
void codegen(Vector *);
void gen(Node *);
void gen_num(Node *);
void gen_lval(Node *);
void gen_ident(Node *);
void gen_assign(Node *);
void gen_if(Node *);
void gen_while(Node *);
void gen_return(Node *);
void gen_funccall(Node *);
void gen_def_func(Node *);
void gen_block(Node *);
void gen_binary_operator(Node *);

// ---util.c---
void error(char *fmt, ...);
Vector *new_vector();
void vec_push(Vector *, void *);
Map *new_map();
void map_set(Map *, char *, void *);
void *map_get(Map *, char *);
bool map_exists(Map *, char *);

// ---util_test.c---
int examine(int, int, int);
void vec_test();
void map_test();

// ---dump.c---
void dump_token(Vector *);
void dump_nodes(Vector *);
void dump_node(Node *, int);
void dump_type(Node *);

#endif
