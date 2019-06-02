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

typedef struct Node
{
	int type;
	int value;
	char* name;
	struct Node *lhs;
	struct Node *rhs;
} Node;

typedef struct Token
{
	int type;
	int value;
	char *name;
	char *input;
} Token;

typedef struct Vector
{
	void **data;
	int capacity;
	int len;
} Vector;

typedef struct Map
{
	Vector *keys;
	Vector *values;
} Map;

enum
{
	TK_NUM = 256,
	TK_EOF,
	TK_EQ,
	TK_NE,
	TK_LE,
	TK_GE,
	TK_IDENT,
	TK_RETURN,
};

enum
{
	ND_NUM = 256,
	ND_IDENT,
	ND_RETURN,
};

// ---main.c---
extern bool is_debug;

// ---tokenize.c---
Token *add_token(Vector *, int, char *);
bool is_alnum(char c);
Vector *tokenize(char *);

// ---parse.c---
extern int variable_offset;
Node *new_node(int, Node *, Node *);
Node *new_node_num(int);
Node *new_node_val(char *name);
int consume(int);
Vector *parse(Vector *, Map *);
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
void codegen(Vector *, Map *);
void gen(Node *);
void gen_lval(Node *);

// ---util.c---
void debug_printf(char *fmt, ...);
void error(char *fmt, ...);
Vector *new_vector();
void vec_push(Vector *, void *);
Map *new_map();
void map_set(Map *, char *, void *);
void *map_get(Map *, char *);
bool map_exists(Map *, char *);

// ---util_test.c---
int expect(int, int, int);
void vec_test();
void map_test();

#endif
