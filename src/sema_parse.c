#include "ycc.h"

void sema_parse(Node *node) {
    if (!node || (node->c_type && node->node_type != ND_DEF_FUNC))
        return;

    sema_parse(node->lhs);
    sema_parse(node->rhs);
    sema_parse(node->condition);
    sema_parse(node->then);
    sema_parse(node->els);
    sema_parse(node->init);
    sema_parse(node->inc);
    sema_parse(node->body);

    if (node->args != NULL) {
        for (int i = 0; node->args->data[i]; i++) {
            sema_parse(node->args->data[i]);
        }
    }
    if (node->params != NULL) {
        for (int i = 0; node->params->data[i]; i++) {
            sema_parse(node->params->data[i]);
        }
    }
    if (node->stmts_in_block != NULL) {
        for (int i = 0; node->stmts_in_block->data[i]; i++) {
            sema_parse(node->stmts_in_block->data[i]);
        }
    }

    switch (node->node_type) {
    case ND_FUNCCALL:
        node->c_type = new_type(TY_FUNC, 1, NULL);
    case ND_AND:
    case ND_OR:
    case ND_EQ:
    case ND_NE:
    case ND_LESS:
    case ND_LE:
    case ND_MUL:
    case ND_DIV:
    case ND_NOT:
        node->c_type = new_type(TY_INT, 4, NULL);
        break;
    case ND_ASSIGN:
    case ND_ADDR:
        node->c_type = node->lhs->c_type;
        break;
    case ND_DEREF:
        node->c_type = node->lhs->c_type->ptr_to;
        break;
    case ND_ADD:
        // Convert 'num + ptr' to 'ptr + num'
        if (node->rhs->c_type->type == TY_PTR) {
            Node *tmp = node->lhs;
            node->lhs = node->rhs;
            node->rhs = tmp;
        }
        // 'ptr + ptr' is invalid, and its rhs is still 'ptr' after swapping
        if (node->rhs->c_type->type == TY_PTR)
            ERROR("Invalid operands to binary expression ('int *' and 'int *')\n");
        node->c_type = node->lhs->c_type;
    case ND_SUB:
        if (node->lhs->c_type->type == TY_PTR) {
            if (node->rhs->c_type->type == TY_PTR)
                node->c_type->type = TY_INT;
        }
        else if (node->rhs->c_type->type == TY_PTR)
            ERROR("Invalid operands to binary expression ('int' and 'int *')\n");
        node->c_type = node->lhs->c_type;
        break;
    case ND_SIZEOF:
        node->c_type = new_type(TY_INT, 4, NULL);
    case ND_NUM:
        node->c_type = new_type(TY_INT, 4, NULL);
        break;
    default:
        break;
    }
}

void sema_parse_nodes(Vector *nodes) {
    for (int i = 0; nodes->data[i]; i++) {
        sema_parse(nodes->data[i]);
    }
}
