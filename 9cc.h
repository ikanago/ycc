#ifndef _9CC_H_
#define _9CC_H_
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
	int type;
	int value;
	struct Node *lhs;
	struct Node *rhs;
} Node;

typedef struct Token
{
	int type;
	int value;
	char *input;
} Token;

typedef struct Vector
{
	void **data;
	int capacity;
	int len;
} Vector;

enum
{
	TK_NUM = 256,
	TK_EOF,
	TK_EQ,
	TK_NE,
	TK_LE,
	TK_GE,
};

enum
{
	ND_NUM = 256,
};

Vector *new_vector();
void vec_push(Vector *, void *);
int expect(int, int, int);
void vec_test();

Node *new_node(int, Node *, Node *);
Node *new_node_num(int);
int consume(int);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *parse(Vector *);
Token *add_token(Vector *, int, char *);
void error(char *fmt, ...);
void gen(Node *);
Vector *tokenize(char *);

#endif
