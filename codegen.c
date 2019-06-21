#include "ycc.h"

int label_else_number = 0;
int label_end_number = 0;

void codegen(Vector *nodes) {
    for (int i = 0; nodes->data[i]; i++) {
        label_end_number++;
        gen(nodes->data[i]);
        printf("  pop rax\n");
        printf("# end of a statement\n\n");
    }
}

void gen(Node *node) {
    switch (node->type) {
    case ND_NUM:
        gen_num(node);
        break;
    case ND_IDENT:
        gen_ident(node);
        break;
    case '=':
        gen_assign(node);
        break;
    case ND_RETURN:
        gen_return(node);
        break;
    case ND_IF:
        gen_if(node);
        break;
    case ND_BLOCK:
        gen_block(node);
        break;
    default:
        gen_binary_operator(node);
    }
}

void gen_num(Node *node) {
    printf("  push %d\n", node->value);
    printf("# num: %d\n", node->value);
    return;
}

void gen_lval(Node *node) {
    if (node->type != ND_IDENT)
        error("Left value of assignment is not variable.");

    int *offset = (int *)map_get(variable_map, node->name);
    map_set(variable_map, node->name, (void *)offset);

    printf("  mov rax, rbp\n");
    printf("  sub rax, %ld\n", (intptr_t)offset);
    printf("  push rax\n");
    printf("# left value: %s\n", node->name);
}

void gen_ident(Node *node) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    printf("# variable: %s\n", node->name);
    return;
}

void gen_assign(Node *node) {
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    printf("# =\n");
    return;
}

void gen_return(Node *node) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    printf("# return\n");
    return;
}

void gen_if(Node *node) {
    if (node->els) {
        label_else_number++;
        gen(node->condition);
        printf("# if-condition\n");
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", label_else_number);
        gen(node->then);
        printf("# if-then\n");

        printf("  jmp .Lend%d\n", label_end_number);
        printf(".Lelse%d:\n", label_else_number);
        gen(node->els);
        printf("# else\n");
        printf(".Lend%d:\n", label_end_number);
    }
    else {
        gen(node->condition);
        printf("# if-condition\n");
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", label_end_number);
        gen(node->then);
        printf(".Lend%d:\n", label_end_number);
    }
    printf("  push rax\n");
    return;
}

void gen_block(Node *node) {
    Vector *stmts = node->stmts_in_block;
    for (int i = 0; stmts->data[i]; i++) {
        gen(stmts->data[i]);
        printf("  pop rax\n");
        printf("# end of a statement\n\n");
    }
    return;
}

void gen_binary_operator(Node *node) {
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