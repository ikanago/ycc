#include "9cc.h"

int pos = 0;
Token tokens[100];
//Vector *tokens = new_vector();

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


