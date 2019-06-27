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
    if (!map_exists(variable_map, name)) {
        variable_offset += 8;
        map_set(variable_map, name, (void *)(intptr_t)variable_offset);
    }
    return node;
}

Node *new_node_funccall(char *name, Vector *args) {
    Node *node = malloc(sizeof(Node));
    node->type = ND_FUNCCALL;
    node->name = name;
    node->args = args;
    return node;
}

Node *new_node_def_func(char *name, Vector *params, Node *body) {
    Node *node = malloc(sizeof(Node));
    node->type = ND_DEF_FUNC;
    node->name = name;
    node->params = params;
    node->body = body;
    return node;
}

void expect(int type) {
    Token *t = tokens->data[token_index];
    if (t->type != type)
        error("%c (%d) expected, but got %c (%d) at %s", type, type, t->type,
              t->type, t->input);
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
    return program();
}

Vector *program() {
    for (;;) {
        Token *t = tokens->data[token_index];
        if (t->type == TK_EOF)
            break;
        variable_map = new_map();
        variable_offset = 0;
        Node *node = definition();
        node->vars = variable_map;
        node->max_variable_offset = variable_offset;
        vec_push(nodes, node);
    }
    vec_push(nodes, NULL);
    return nodes;
}

Node *definition() { return define_func(); }

Node *define_func() {
    Token *t = tokens->data[token_index];
    if (t->type != TK_IDENT)
        error("Not an identifier");
    char *name = t->name;
    token_index++;

    Vector *params = func_params();
    Node *body = stmt();
    Node *node = new_node_def_func(name, params, body);
    return node;
}

Vector *func_params() {
    Vector *params = new_vector();
    if (!consume('(')) {
        Token *t = tokens->data[token_index];
        error("Expected '(', but got: %s", t->input);
    }
    if (consume(')'))
        return params;

    bool is_first_param = true;
    while (!consume(')')) {
        if (!is_first_param)
            if (!consume(','))
                error("Expected ','");
        is_first_param = false;

        Token *t = tokens->data[token_index];
        if (!consume(TK_IDENT))
            error("Expexted identifier.");
        Node *node = new_node_var(t->name);
        vec_push(params, node);
    }
    return params;
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
        if (consume(TK_ELSE))
            node->els = stmt();
    }
    else if (consume('{')) {
        node = malloc(sizeof(Node));
        node->type = ND_BLOCK;
        node->stmts_in_block = new_vector();
        while (!consume('}'))
            vec_push(node->stmts_in_block, stmt());
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
            node = new_node(ND_EQ, node, relational());
        }
        else if (consume(TK_NE)) {
            node = new_node(ND_NE, node, relational());
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
            node = new_node(ND_LE, node, add());
        }
        else if (consume('<')) {
            node = new_node('<', node, add());
        }
        else if (consume(TK_GE)) {
            node = new_node(ND_LE, add(), node);
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
            error("Expected ')', but got: %s", t->input);
        return node;
    }
    else if (t->type == TK_NUM) {
        token_index++;
        return new_node_num(t->value);
    }
    else if (t->type == TK_IDENT) {
        token_index++;
        char *name = t->name;
        if (!consume('(')) {
            return new_node_var(name);
        }

        Vector *args = new_vector();
        if (consume(')')) {
            return new_node_funccall(name, args);
        }

        vec_push(args, expr());
        while (consume(','))
            vec_push(args, expr());
        expect(')');
        return new_node_funccall(name, args);
    }
    else {
        error("Expected value, but got: %s", t->input);
        return NULL;
    }
}