#include "ycc.h"

int g_label_else_number = 0;
int g_label_begin_number = 0;
int g_label_end_number = 0;
// registers to store function parameters and arguments.
char *g_registers_for_args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
Map *g_variable_map;

void gen_num(Node *node) {
    printf("  push %d\n", node->value);
    printf("# num: %d\n", node->value);
}

void gen_lval(Node *node) {
    if (node->type != ND_IDENT)
        ERROR("Left value of assignment is not variable.");

    int *offset = (int *)map_get(g_variable_map, node->name);
    map_set(g_variable_map, node->name, (void *)offset);

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
}

void gen_assign(Node *node) {
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    printf("# =\n");
}

void gen_if(Node *node) {
    g_label_else_number++;
    g_label_end_number++;
    if (node->els) {
        gen(node->condition);
        printf("# if-condition\n");
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", g_label_else_number);
        gen(node->then);
        printf("# if-then\n");

        printf("  jmp .Lend%d\n", g_label_end_number);
        printf(".Lelse%d:\n", g_label_else_number);
        gen(node->els);
        printf("# else\n");
        printf(".Lend%d:\n", g_label_end_number);
    }
    else {
        gen(node->condition);
        printf("# if-condition\n");
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", g_label_end_number);
        gen(node->then);
        printf(".Lend%d:\n", g_label_end_number);
    }
    printf("  push rax\n");
}

void gen_while(Node *node) {
    g_label_begin_number++;
    g_label_end_number++;
    printf(".Lbegin%d:\n", g_label_begin_number);
    gen(node->condition);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", g_label_end_number);
    gen(node->body);
    printf("  jmp .Lbegin%d\n", g_label_begin_number);
    printf(".Lend%d:\n", g_label_end_number);
}

void gen_for(Node *node) {
    g_label_begin_number++;
    g_label_end_number++;
    gen(node->init);
    printf(".Lbegin%d:\n", g_label_begin_number);
    gen(node->condition);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", g_label_end_number);
    gen(node->body);
    gen(node->inc);
    printf("  jmp .Lbegin%d\n", g_label_begin_number);
    printf(".Lend%d:\n", g_label_end_number);
}

void gen_funccall(Node *node) {
    for (int i = node->args->len - 1; i >= 0; i--) {
        gen(node->args->data[i]);
        printf("  pop %s\n", g_registers_for_args[i]);
        printf("# store arg to regester\n");
    }
    printf("  call %s\n", node->name);
    printf("  push rax\n");
    printf("# function call\n");
}

void gen_def_func(Node *node) {
    g_variable_map = node->vars;
    printf("%s:\n", node->name);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", node->max_variable_offset);
    printf("# prologue\n");

    for (int i = 0; i < node->params->len; i++) {
        Node *param = node->params->data[i];
        int offset = (intptr_t)map_get(g_variable_map, param->name);
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", offset);
        printf("  mov [rax], %s\n", g_registers_for_args[i]);
        printf("# store parameters to stack\n");
    }

    gen(node->body);

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

void gen_return(Node *node) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    printf("# return\n");
}

void gen_block(Node *node) {
    Vector *stmts = node->stmts_in_block;
    for (int i = 0; stmts->data[i]; i++) {
        gen(stmts->data[i]);
        printf("  pop rax\n");
        printf("# end of a statement\n\n");
    }
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
    case ND_IF:
        gen_if(node);
        break;
    case ND_WHILE:
        gen_while(node);
        break;
    case ND_FOR:
        gen_for(node);
        break;
    case ND_FUNCCALL:
        gen_funccall(node);
        break;
    case ND_DEF_FUNC:
        gen_def_func(node);
        break;
    case ND_RETURN:
        gen_return(node);
        break;
    case ND_BLOCK:
        gen_block(node);
        break;
    default:
        gen_binary_operator(node);
    }
}

void codegen(Vector *nodes) {
    printf(".intel_syntax noprefix\n");
    for (int i = 0; nodes->data[i]; i++) {
        g_label_end_number++;
        Node *node = nodes->data[i];
        printf(".global %s\n", node->name);
        gen(node);
        printf("  pop rax\n");
        printf("# end of a statement\n\n");
    }
    printf("# end of a program\n");
}