#include "ycc.h"

// registers to store function parameters and arguments.
char *g_registers64_for_args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
char *g_registers32_for_args[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
Map *g_lvar_offset_map;
extern Vector *g_gvar_nodes;

void gen_num(Node *node) {
    printf("  push %d\n", node->value);
    printf("# num: %d\n", node->value);
}

void gen_lval(Node *node) {
    if (node->node_type == ND_LVAR) {
        int *offset = (int *)map_get(g_lvar_offset_map, node->name);
        printf("  lea rax, [rbp-%ld]\n", (intptr_t)offset);
        printf("  push rax\n");
    }
    else if (node->node_type == ND_GVAR) {
        printf("  lea rax, %s[rip]\n", node->name);
        printf("  push rax\n");
    }
    else if (node->node_type == ND_DEREF) {
        gen(node->lhs);
    }
    printf("# left value address: %s\n", node->name);
}

// Generate assembly to load value from a register
// corresponding to each type size.
void gen_load_value(Node *node) {
    while (node->node_type == ND_DEREF)
        node = node->lhs;

    printf("  pop rax\n");
    if (node->c_type->size == 1)
        printf("  mov al, BYTE PTR [rax]\n");
    if (node->c_type->size == 4)
        printf("  mov eax, DWORD PTR [rax]\n");
    if (node->c_type->size == 8)
        printf("  mov rax, [rax]\n");
    printf("  push rax\n");
}

void gen_store_value(Node *node) {
    while (node->node_type == ND_DEREF)
        node = node->lhs;

    printf("  pop rdi\n");
    printf("  pop rax\n");
    if (node->c_type->size == 1)
        printf("  mov BYTe PTR [rax], dil\n");
    if (node->c_type->size == 4)
        printf("  mov DWORD PTR [rax], edi\n");
    else if (node->c_type->size == 8)
        printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
}

void gen_dereference(Node *node) {
    gen(node->lhs);
    gen_load_value(node);
    printf("# dereference\n");
}

void gen_ident(Node *node) {
    gen_lval(node);
    // Not array variable
    if (node->c_type->type != TY_ARRAY)
        gen_load_value(node);
    printf("# variable: %s\n", node->name);
}

void gen_assign(Node *node) {
    gen_lval(node->lhs);
    gen(node->rhs);
    gen_store_value(node->lhs);
    printf("# assignment to \"%s\"\n", node->lhs->name);
}

void gen_logical_not(Node *node) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
}

void gen_logical_or(Node *node) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .Ltrue%p\n", &(node));
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .Ltrue%p\n", &(node));
    printf("  push 0\n");
    printf("  jmp .Lend_or%p\n", &(node));
    printf(".Ltrue%p:\n", &(node));
    printf("  push 1\n");
    printf(".Lend_or%p:\n", &(node));
}

void gen_logical_and(Node *node) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lfalse%p\n", &(node));
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .Ltrue%p\n", &(node));
    printf(".Lfalse%p:\n", &(node));
    printf("  push 0\n");
    printf("  jmp .Lend_and%p\n", &(node));
    printf(".Ltrue%p:\n", &(node));
    printf("  push 1\n");
    printf(".Lend_and%p:\n", &(node));
}

void gen_if(Node *node) {
    gen(node->condition);
    printf("# if-condition\n");
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%p\n", &(node->els));
    gen(node->then);
    printf("# if-then\n");

    if (node->els) {
        printf("  jmp .Lend_if%p\n", &(node->els));
        printf(".Lelse%p:\n", &(node->els));
        gen(node->els);
        printf("# else\n");
        printf(".Lend_if%p:\n", &(node->els));
        printf("# if-else\n");
    }
    else {
        printf(".Lelse%p:\n", &(node->els));
    }

    printf("  push rax\n");
}

void gen_while(Node *node) {
    printf(".Lbegin_while%p:\n", &(node));
    gen(node->condition);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend_while%p\n", &(node));
    gen(node->body);
    printf("  jmp .Lbegin_while%p\n", &(node));
    printf(".Lend_while%p:\n", &(node));
}

void gen_for(Node *node) {
    gen(node->init);
    printf(".Lbegin_for%p:\n", &(node));
    gen(node->condition);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend_for%p\n", &(node));
    gen(node->body);
    gen(node->inc);
    printf("  jmp .Lbegin_for%p\n", &(node));
    printf(".Lend_for%p:\n", &(node));
}

void gen_funccall(Node *node) {
    for (int i = node->args->len - 1; i >= 0; i--) {
        Node *argument = node->args->data[i];
        gen(argument);
        printf("  pop %s\n", g_registers64_for_args[i]);
        printf("# store arg to register\n");
    }
    printf("  call %s\n", node->name);
    printf("  push rax\n");
    printf("# function call\n");
}

void gen_def_func(Node *node) {
    g_lvar_offset_map = node->lvar_offset_map;
    printf("%s:\n", node->name);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", node->max_variable_offset);
    printf("# prologue\n");

    for (int i = 0; i < node->params->len; i++) {
        Node *param = node->params->data[i];
        int offset = (intptr_t)map_get(g_lvar_offset_map, param->name);
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", offset);
        if (param->c_type->size == 4)
            printf("  mov DWORD PTR [rax], %s\n", g_registers32_for_args[i]);
        else if (param->c_type->size == 8)
            printf("  mov [rax], %s\n", g_registers64_for_args[i]);
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
    printf("  push rax\n");
    printf("# end of of a block\n\n");
}

void gen_sizeof(Node *node) {
    if (node->lhs->c_type->type == TY_ARRAY)
        printf("  push %d\n", node->lhs->c_type->array_size);
    else
        printf("  push %d\n", node->lhs->c_type->size);
    printf("# sizeof\n");
}

void gen_binary_operator(Node *node) {
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->node_type) {
    case ND_ADD:
        if (node->c_type->type == TY_PTR)
            printf("  imul rdi, %d\n", node->lhs->c_type->ptr_to->size);
        else if (node->c_type->type == TY_ARRAY)
            printf("  imul rdi, %d\n", node->lhs->c_type->array_of->size);
        printf("  add rax, rdi\n");
        printf("# +\n");
        break;
    case ND_SUB:
        if (node->c_type->type == TY_PTR)
            printf("  imul rdi, %d\n", node->lhs->c_type->ptr_to->size);
        else if (node->c_type->type == TY_ARRAY)
            printf("  imul rdi, %d\n", node->lhs->c_type->array_of->size);
        printf("  sub rax, rdi\n");
        printf("# -\n");
        break;
    case ND_MUL:
        printf("  mul rdi\n");
        printf("# *\n");
        break;
    case ND_DIV:
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
    case ND_LESS:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

void gen(Node *node) {
    switch (node->node_type) {
    case ND_NUM:
        gen_num(node);
        break;
    case ND_LVAR:
    case ND_GVAR:
        gen_ident(node);
        break;
    case ND_ASSIGN:
        gen_assign(node);
        break;
    case ND_NOT:
        gen_logical_not(node);
        break;
    case ND_OR:
        gen_logical_or(node);
        break;
    case ND_AND:
        gen_logical_and(node);
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
    case ND_ADDR:
        gen_lval(node->lhs);
        break;
    case ND_DEREF:
        gen_dereference(node);
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
    case ND_SIZEOF:
        gen_sizeof(node);
        break;
    default:
        gen_binary_operator(node);
    }
}

void codegen(Vector *nodes) {
    printf(".intel_syntax noprefix\n");
    for (int i = 0; g_gvar_nodes->data[i]; i++) {
        Node *node = g_gvar_nodes->data[i];
        if (node->node_type == ND_ASSIGN) {
            printf("%s:\n", node->lhs->name);
            if (node->lhs->c_type->size == 4)
                printf("  .long %d\n", node->rhs->value);
            else if (node->lhs->c_type->size == 8)
                printf("  .quad %d\n", node->rhs->value);
        }
        else if (node->node_type == ND_GVAR) {
            printf(".comm %s,%d\n", node->name, node->c_type->size);
        }
    }
    for (int i = 0; nodes->data[i]; i++) {
        Node *node = nodes->data[i];
        printf(".global %s\n", node->name);
        gen(node);
        printf("  pop rax\n");
        printf("# end of a statement\n\n");
    }
    printf("# end of a program\n");
}
