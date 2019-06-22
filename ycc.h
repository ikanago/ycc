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
    int type;
    char *type_name;
    int value;
    char *token_string;
    char *input;
} Token;

typedef struct Node {
    int type;
    int value;
    char *name;
    struct Node *lhs;
    struct Node *rhs;
    struct Vector *stmts_in_block;
    struct Node *condition;
    struct Node *then;
    struct Node *els;
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

enum {
    TK_NUM = 256, // Number literal
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
    TK_IDENT,     // Ifentifier
    TK_IF,        // if
    TK_ELSE,      // else
    TK_FUNCCALL,  // function call
    TK_RETURN,    // return
    TK_EOF,       // End Of Statement
};

enum {
    ND_NUM = 512, // Number literal
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LE,        // <=
    ND_GE,        // >=
    ND_IDENT,     // Ifentifier
    ND_IF,        // if
    ND_RETURN,    // return
    ND_FUNCCALL,  // function call
    ND_BLOCK,     // { }
};

// ---main.c---
extern Map *variable_map;
extern int variable_offset;

// ---tokenize.c---
Token *add_token(Vector *, int, char *);
bool is_alnum(char c);
Vector *tokenize(char *);
Vector *scan(char *);

// ---parse.c---
extern int variable_offset;
Node *new_node(int, Node *, Node *);
Node *new_node_num(int);
Node *new_node_val(char *name);
void expect(int);
int consume(int);
Vector *parse(Vector *);
Vector *program();
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
void gen_funccall(Node *);
void gen_return(Node *);
void gen_block(Node *);
void gen_binary_operator(Node *);

// ---util.c---
void error(char *fmt, ...);
void dump_token(Vector *);
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

#endif
