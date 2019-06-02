#include "ycc.h"

extern Map *variable_map;
extern int variable_offset;

void codegen(Vector *nodes, Map *map)
{
	variable_map = map;
	for (int i = 0; nodes->data[i]; i++)
	{
		gen(nodes->data[i]);
		printf("  pop rax\n");
		debug_printf("; end of a statement\n\n");
	}
}

void gen(Node *node)
{
	if (node->type == ND_NUM)
	{
		printf("  push %d\n", node->value);
		debug_printf("; num: %d\n", node->value);
		return;
	}
	else if (node->type == ND_IDENT)
	{
		gen_lval(node);
		printf("  pop rax\n");
		printf("  mov rax, [rax]\n");
		printf("  push rax\n");
		debug_printf("; variable: %s\n", node->name);
		return;
	}
	else if (node->type == '=')
	{
		gen_lval(node->lhs);
		gen(node->rhs);
		printf("  pop rdi\n");
		printf("  pop rax\n");
		printf("  mov [rax], rdi\n");
		printf("  push rdi\n");
		debug_printf("; =\n");
		return;
	}
	else if (node->type == ND_RETURN)
	{
		gen(node->lhs);
		printf("  pop rax\n");
		printf("  mov rsp, rbp\n");
		printf("  pop rbp\n");
		printf("  ret\n");
		debug_printf("; return\n");
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->type)
	{
	case '+':
		printf("  add rax, rdi\n");
		debug_printf("; +\n");
		break;
	case '-':
		printf("  sub rax, rdi\n");
		debug_printf("; -\n");
		break;
	case '*':
		printf("  mul rdi\n");
		debug_printf("; *\n");
		break;
	case '/':
		printf("  mov rdx, 0\n");
		printf("  div rdi\n");
		debug_printf("; /\n");
		break;
	case TK_EQ:
		printf("  cmp rax, rdi\n");
		printf("  sete al\n");
		printf("  movzb rax, al\n");
		break;
	case TK_NE:
		printf("  cmp rax, rdi\n");
		printf("  setne al\n");
		printf("  movzb rax, al\n");
		break;
	case TK_LE:
		printf("  cmp rax, rdi\n");
		printf("  setle al\n");
		printf("  movzb rax, al\n");
		break;
	case '<':
		printf("  cmp rax, rdi\n");
		printf("  setl al\n");
		printf("  movzb rax, al\n");
		break;
	}

	printf("  push rax\n");
}

void gen_lval(Node *node)
{
	if (node->type != ND_IDENT)
		error("Left value of assignment is not variable.");

	int offset = (intptr_t)map_get(variable_map, node->name);
	map_set(variable_map, node->name, (void *)offset);

	printf("  mov rax, rbp\n");
	printf("  sub rax, %d\n", offset);
	printf("  push rax\n");
	debug_printf("; left value\n");
}