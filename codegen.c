#include "ycc.h"

int label_number = 0;

void codegen(Vector *nodes) {
    for (int i = 0; nodes->data[i]; i++) {
        gen(nodes->data[i]);
        printf("  pop rax\n");
        printf("# end of a statement\n\n");
    }
}

void gen(Node *node) {
    if (node->type == ND_NUM) {
        printf("  push %d\n", node->value);
        printf("# num: %d\n", node->value);
        return;
    }
    else if (node->type == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        printf("# variable: %s\n", node->name);
        return;
    }
    else if (node->type == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        printf("# =\n");
        return;
    }
    else if (node->type == ND_RETURN) {
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        printf("# return\n");
        return;
    }
    else if (node->type == ND_IF) {
        gen(node->condition);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        label_number++;
        printf("  je .Lend%06d\n", label_number);
        gen(node->then);
        printf(".Lend%06d:\n", label_number);
        printf("  push rax\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->type) {
    case '+':
        printf("  add rax, rdi\n");
        printf("# +\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        printf("# -\n");
        break;
    case '*':
        printf("  mul rdi\n");
        printf("# *\n");
        break;
    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
        printf("# /\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
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

void gen_lval(Node *node) {
    if (node->type != ND_IDENT)
        error("Left value of assignment is not variable.");

    int *offset = (int *)map_get(variable_map, node->name);
    map_set(variable_map, node->name, (void *)offset);

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", *offset);
    printf("  push rax\n");
    printf("# left value: %s\n", node->name);
}