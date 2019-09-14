#include "ycc.h"

void dump_token(Vector *tokens) {
    printf("\n-----DUMP-TOKEN-----\n");
    int i = 0;
    while (true) {
        Token *t = tokens->data[i];
        if (t->type == TK_EOF)
            return;
        printf("\033[0;32m");
        printf("%-10s", t->type_name);
        printf("\033[0m");
        if (t->value)
            printf(" -> %d\n", t->value);
        else
            printf(" -> %s\n", t->name);
        i++;
    }
}

void emit_space(int num) {
    for (int i = 0; i < num; i += 4)
        printf("    ");
}

void dump_C_type(Node *node) {
    if (node->c_type == NULL)
        return;
    printf(": ");
    switch (node->c_type->type) {
    case TY_CHAR:
        printf("CHAR/%d", node->c_type->size);
    case TY_INT:
        printf("INT/%d", node->c_type->size);
        break;
    case TY_ARRAY:
        printf("ARRAY");
        printf("/SIZE: %dx%d", node->c_type->array_of->size, node->c_type->array_len);
        break;
    case TY_PTR:
        printf("PTR/%d", node->c_type->size);
        break;
    default:
        break;
    }
}

void dump_node_type(Node *node) {
    switch (node->node_type) {
    case ND_NUM:
        printf("%d", node->value);
        break;
    case ND_EQ:
        printf("EQUAL");
        break;
    case ND_NE:
        printf("NOT_EQUAL");
        break;
    case ND_LE:
        printf("LESS/EQUAL");
        break;
    case ND_NOT:
        printf("NOT");
        break;
    case ND_OR:
        printf("OR");
        break;
    case ND_AND:
        printf("AND");
        break;
    case ND_LVAR:
    case ND_GVAR:
        printf("%s", node->name);
        dump_C_type(node);
        break;
    case ND_IF:
        printf("IF");
        break;
    case ND_RETURN:
        printf("RETURN");
        break;
    case ND_WHILE:
        printf("WHILE");
        break;
    case ND_FOR:
        printf("FOR");
        break;
    case ND_ADDR:
        printf("ADDR");
        break;
    case ND_DEREF:
        printf("DEREF");
        break;
    case ND_FUNCCALL:
        printf("%s: call", node->name);
        break;
    case ND_DEF_FUNC:
        printf("%s: define", node->name);
        break;
    case ND_BLOCK:
        printf("BLOCK");
        break;
    case ND_ADD:
        printf("ADD");
        break;
    case ND_SUB:
        printf("SUB");
        break;
    case ND_MUL:
        printf("MUL");
        break;
    case ND_DIV:
        printf("DIV");
        break;
    case ND_LESS:
        printf("GREATER");
        break;
    case ND_ASSIGN:
        printf("ASSIGN");
        break;
    case ND_SIZEOF:
        printf("SIZEOF");
        break;
    case ND_STRING:
        printf("%s", node->string_literal);
        break;
    default:
        printf("%d", node->node_type);
        break;
    }
    printf("\n");
}

void dump_node(Node *node, int depth) {
    int child_depth = depth + 4;
    if (depth != 0) {
        emit_space(depth - 4);
        printf("└───");
    }

    dump_node_type(node);

    if (node->lhs)
        dump_node(node->lhs, child_depth);
    if (node->rhs)
        dump_node(node->rhs, child_depth);
    if (node->init)
        dump_node(node->init, child_depth);
    if (node->condition)
        dump_node(node->condition, child_depth);
    if (node->inc)
        dump_node(node->inc, child_depth);
    if (node->then)
        dump_node(node->then, child_depth);
    if (node->els)
        dump_node(node->els, child_depth);
    if (node->args) {
        emit_space(child_depth - 4);
        printf("ARGS:\n");
        for (int i = 0; node->args->data[i]; i++) {
            dump_node(node->args->data[i], child_depth);
        }
    }
    if (node->params) {
        emit_space(child_depth - 4);
        printf("PARAMS: %d\n", node->params->len);
        for (int i = 0; node->params->data[i]; i++) {
            dump_node(node->params->data[i], child_depth);
        }
    }
    if (node->body)
        dump_node(node->body, child_depth);
    if (node->stmts_in_block) {
        for (int i = 0; node->stmts_in_block->data[i]; i++) {
            dump_node(node->stmts_in_block->data[i], child_depth);
        }
    }
}

void dump_nodes(Vector *nodes) {
    printf("\n-----DUMP-NODES-----\n");
    int i = 0;
    while (nodes->data[i]) {
        dump_node(nodes->data[i], 0);
        printf("--------------------\n");
        i++;
    }
}