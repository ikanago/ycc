#ifndef _9CC_H_
#define _9CC_H_

typedef struct Node {
	int type;
	int value;
	struct Node *lhs;
	struct Node *rhs;
} Node;

typedef struct {
	int type;
	int value;
	char *input;
} Token;

typedef struct Vector {
	void **data;
	int capacity;
	int len;
} Vector;

enum {
	TK_NUM = 256,
	TK_EOF,
};

enum {
	ND_NUM = 256,
};

Vector *new_vector();
void vec_push(Vector*, void*);
Node *new_node(int, Node*, Node*);
Node *new_node_num(int);
int consume(int);
void error(int);
Node *term();
Node *mul();
Node *add();
void gen(Node*);
void tokenize(char*);


#endif
