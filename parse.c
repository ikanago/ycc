#include "ycc.h"

Vector *tokens;
Vector *nodes;
int token_index;
Map *variable_map;
int variable_offset;

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

Node *new_node_var(char *name) {
    Node *node = malloc(sizeof(Node));
    node->type = ND_IDENT;
    node->name = name;
    return node;
}

void expect(int type) {
    Token *t = tokens->data[token_index];
    if (t->type != type)
        error("%c (%d9 expected, but got %c (%d)", type, type, t->type,
              t->type);
    token_index++;
}

int consume(int type) {
    Token *t = tokens->data[token_index];
    if (t->type != type)
        return 0;
    token_index++;
    return 1;
}

Vector *parse(Vector *v) {
    tokens = v;
    nodes = new_vector();
    token_index = 0;
    variable_map = new_map();
    variable_offset = 0;
    return program();
}

Vector *program() {
    for (;;) {
        Token *t = tokens->data[token_index];
        if (t->type == TK_EOF)
            break;
        Node *node = stmt();
        vec_push(nodes, node);
    }
    vec_push(nodes, NULL);
    return nodes;
}

Node *stmt() {
    Node *node;
    if (consume(TK_RETURN)) {
        node = new_node(ND_RETURN, expr(), NULL);
        expect(';');
    }
    else if (consume(TK_IF)) {
        node = malloc(sizeof(Node));
        node->type = ND_IF;
        expect('(');
        node->condition = assign();
        expect(')');
        node->then = stmt();
    }
    else {
        node = expr();
        expect(';');
    }
    return node;
}

Node *expr() {
    Node *node = assign();
    return node;
}

Node *assign() {
    Node *node = equality();
    if (consume('=')) {
        node = new_node('=', node, assign());
    }
    return node;
}

Node *equality() {
    Node *node = relational();
    for (;;) {
        if (consume(TK_EQ)) {
            node = new_node(TK_EQ, node, relational());
        }
        else if (consume(TK_NE)) {
            node = new_node(TK_NE, node, relational());
        }
        else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = add();
    for (;;) {
        if (consume(TK_LE)) {
            node = new_node(TK_LE, node, add());
        }
        else if (consume('<')) {
            node = new_node('<', node, add());
        }
        else if (consume(TK_GE)) {
            node = new_node(TK_LE, add(), node);
        }
        else if (consume('>')) {
            node = new_node('<', add(), node);
        }
        else {
            return node;
        }
    }
}

Node *add() {
    Node *node = mul();
    for (;;) {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();
    for (;;) {
        if (consume('*'))
            node = new_node('*', node, unary());
        else if (consume('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
    return term();
}

Node *term() {
    Token *t = tokens->data[token_index];
    if (consume('(')) {
        Node *node = expr();
        if (!consume(')'))
            printf("There is no close parenthesis corresponds open one: %s",
                   t->input);
        return node;
    }
    else if (t->type == TK_NUM) {
        token_index++;
        return new_node_num(t->value);
    }
    else if (t->type == TK_IDENT) {
        token_index++;
        Node *node = new_node_var(t->token_string);
        if (!map_exists(variable_map, t->token_string)) {
            variable_offset += 8;
            map_set(variable_map, t->token_string,
                    (void *)(intptr_t)variable_offset);
        }
        return node;
    }
    else {
        error("Expected value, but got %s", t->input);
        return NULL;
    }
}