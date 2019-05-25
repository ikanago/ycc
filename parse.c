#include "9cc.h"

Vector *tokens;
int pos;

Node *new_node(int type, Node *lhs, Node *rhs)
{
	Node *node = malloc(sizeof(Node));
	node->type = type;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}
Node *new_node_num(int value)
{
	Node *node = malloc(sizeof(Node));
	node->type = ND_NUM;
	node->value = value;
	return node;
}

int consume(int type)
{
	Token *t = tokens->data[pos];
	if (t->type != type)
		return 0;
	pos++;
	return 1;
}

Node *expr()
{
	Node *node = mul();

	for (;;)
	{
		if (consume('+'))
			node = new_node('+', node, mul());
		else if (consume('-'))
			node = new_node('-', node, mul());
		else
			return node;
	}
}

Node *mul()
{
	Node *node = unary();
	for (;;)
	{
		if (consume('*'))
			node = new_node('*', node, unary());
		else if (consume('/'))
			node = new_node('/', node, unary());
		else
			return node;
	}
}

Node *unary()
{
	if (consume('+'))
		return term();
	if (consume('-'))
		return new_node('-', new_node_num(0), term());
	return term();
}

Node *term()
{
	Token *t = tokens->data[pos];
	if (consume('('))
	{
		Node *node = expr();
		if (!consume(')'))
			printf("There is no close parenthese corresponds open one: %s", t->input);
		return node;
	}
	else if (t->type == TK_NUM)
	{
		Token *next_t = tokens->data[pos++];
		return new_node_num(next_t->value);
	}
	else
	{
		printf("This is token which is neither value nor open parenthese: %s",
			   t->input);
	}
}

Node *parse(Vector *v)
{
	tokens = v;
	pos = 0;
	return expr();
}