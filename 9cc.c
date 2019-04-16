#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

int pos = 0;
Token tokens[100];

Vector *new_vector() {
	Vector *vec = malloc(sizeof(Vector));
	vec->data = malloc(sizeof(void *) * 16);
	vec->capacity = 16;
	vec->len = 0;
	return vec;
}

void vec_push(Vector *vec, void *elem) {
	if (vec->capacity == vec->len) {
		vec->capacity *= 2;
		vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
	}
	vec->data[vec->len++] = elem;
}

int expect(int line, int expected, int actual) {
	if (expected == actual)
		return;
	fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
	exit(1);
}

void vec_test() {
	Vector *vec = new_vector();
	expect(__LINE__, 0, vec->len);

	for (int i = 0; i < 100; i++)
		vec_push(vec, (void *)i);

	expect(__LINE__, 100, vec->len);
	expect(__LINE__, 0, (int)vec->data[0]);
	expect(__LINE__, 50, (int)vec->data[50]);
	expect(__LINE__, 99, (int)vec->data[99]);

	printf("OK\n");
}

Node *new_node(int type, Node *lhs, Node *rhs) {
	Node *node = malloc(sizeof(Node));
	node->type = type;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}
Node *new_node_num(int value) {
	Node *node = malloc(sizeof(Node));
	node->type = ND_NUM;
	node->value = value;
	return node;
}


int consume(int type) {
	if (tokens[pos].type != type)
		return 0;
	pos++;
	return 1;
}

void error(int i) {
	fprintf(stderr, "Unexpected token: %s\n", tokens[i].input);
	exit(1);
}

Node *term() {
	if (consume('(')) {
		Node *node = add();
		if(!consume(')'))
			printf("There is no close parenthese corresponds open one: %s",
					tokens[pos].input);
		return node;
	}

	if (tokens[pos].type == TK_NUM)
		return new_node_num(tokens[pos++].value);

    printf("This is token which is neither value nor open parenthese: %s",
			tokens[pos].input);
}

Node *mul() {
	Node *node = term();
    for(;;) {
		if (consume('*'))
			node = new_node('*', node, mul());
		else if (consume('/'))
			node = new_node('/', node, mul());
		else
			return node;
	}
}

Node *add() {
	Node *node = mul();

	for(;;) {
		if (consume('+'))
			node = new_node('+', node, mul());
		else if (consume('-'))
			node = new_node('-', node, mul());
		else
			return node;
	}
}

void gen(Node *node) {
	if (node->type == ND_NUM) {
		printf("  push %d\n", node->value);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->type){
		case '+':
			printf("  add rax, rdi\n");
			break;
		case '-':
			printf("  sub rax, rdi\n");
			break;
		case '*':
			printf("  mul rdi\n");
			break;
		case '/':
			printf("  mov rdx, 0\n");
			printf("  div rdi\n");
	}

	printf("  push rax\n");
}

void tokenize(char *p) {
	int i = 0;
	while (*p) {
		if(isspace(*p)) {
			p++;
		}
		else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
			tokens[i].type = *p;
			tokens[i].input = p;
			i++;
			p++;
		}
		else if (isdigit(*p)) {
			tokens[i].type = TK_NUM;
			tokens[i].input = p;
			tokens[i].value = strtol(p, &p, 10);
			i++;
		}
		else {
			fprintf(stderr, "Cannot tokenize: %s\n", p);
			exit(1);
		}
	}
	tokens[i].type = TK_EOF;
	tokens[i].input = p;
}


