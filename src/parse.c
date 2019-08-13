#include "ycc.h"

Vector *g_tokens;      // token array received from tokenizer.
Vector *g_nodes;       // node array to store parsed node.
int g_token_index;     // index to indicate current position at `g_tokens`.
Map *g_var_offset_map; // map to store sets of local variable and stack offset.
Map *g_var_type_map;   // map to store sets of local variable and its type.
int g_variable_offset; // sum of variable's offset.

Node *new_node(int type, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->node_type = type;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int value) {
    Node *node = malloc(sizeof(Node));
    node->node_type = ND_NUM;
    node->value = value;
    return node;
}

Node *new_node_var(char *name) {
    Node *node = malloc(sizeof(Node));
    node->node_type = ND_IDENT;
    node->name = name;
    node->c_type = (C_type *)map_get(g_var_type_map, name);
    if (!map_exists(g_var_offset_map, name)) {
        g_variable_offset += node->c_type->size;
        map_set(g_var_offset_map, name, (void *)(intptr_t)g_variable_offset);
    }
    return node;
}

Node *new_node_funccall(char *name, Vector *args) {
    Node *node = malloc(sizeof(Node));
    node->node_type = ND_FUNCCALL;
    node->name = name;
    node->args = args;
    return node;
}

Node *new_node_def_func(char *name, C_type *type, Vector *params, Node *body) {
    Node *node = malloc(sizeof(Node));
    node->node_type = ND_DEF_FUNC;
    node->name = name;
    node->c_type = type;
    node->params = params;
    node->body = body;
    return node;
}

// Check if current token's type is expected one.
void expect_token(int type) {
    Token *t = g_tokens->data[g_token_index];
    if (t->type != type)
        ERROR("%c (%d) expected, but got %c (%d) at \"%s\"", type, type,
              t->type, t->type, t->input);
    g_token_index++;
}

// Check if current token's type is desired one.
// If so, proceed to next token.
int consume_next_token(int type) {
    Token *t = g_tokens->data[g_token_index];
    if (t->type != type)
        return 0;
    g_token_index++;
    return 1;
}

// Check if current token's type is desired one.
// Never proceed to next token.
int check_next_token(int type) {
    Token *t = g_tokens->data[g_token_index];
    if (t->type != type)
        return 0;
    return 1;
}

// Generate new C_type struct following information passed by arguments.
// After generating, proceed to next token.
C_type *new_type(int TY_type, int size, C_type *ptr_to) {
    C_type *type = malloc(sizeof(C_type));
    type->type = TY_type;
    type->size = size;
    type->ptr_to = ptr_to;
    while (check_next_token('*'))
        type = new_type(TY_PTR, 8, type);
    return type;
}

C_type *type_specifier() {
    Token *t = g_tokens->data[g_token_index];
    if (consume_next_token(TK_INT)) {
        return new_type(TY_INT, 4, NULL);
    }
    else {
        ERROR("Expected type name, but got \"%s\"", t->input);
    }
}

// Initialize parser and start parsing.
Vector *parse(Vector *v) {
    g_tokens = v;
    g_nodes = new_vector();
    g_token_index = 0;
    return program();
}

// Entry point of constructing syntax tree.
// Top node of each tree will be function type node.
Vector *program() {
    for (;;) {
        Token *t = g_tokens->data[g_token_index];
        if (t->type == TK_EOF)
            break;

        g_var_offset_map = new_map();
        g_var_type_map = new_map();
        g_variable_offset = 0;
        Node *node = definition();
        node->vars = g_var_offset_map;
        node->max_variable_offset = g_variable_offset;
        vec_push(g_nodes, node);
    }
    vec_push(g_nodes, NULL);
    return g_nodes;
}

Node *definition() { return define_func(); }

Node *define_func() {
    C_type *type = type_specifier();
    Token *t = g_tokens->data[g_token_index];
    if (t->type != TK_IDENT)
        ERROR("Expected identifier name, but got \"%s\"", t->input);
    char *func_name = t->name;
    g_token_index++;

    Vector *params = func_params();
    Node *body = stmt();
    Node *node = new_node_def_func(func_name, type, params, body);
    return node;
}

Node *param() {
    // C_type *type = type_specifier();
    // Token *t = g_tokens->data[g_token_index];
    // Node *node = new_node_var(t->name);
    // node->c_type = type;
    // g_token_index++;
    Node *node = decl_var();
    return node;
}

Vector *func_params() {
    Vector *params = new_vector();
    if (!consume_next_token('(')) {
        Token *t = g_tokens->data[g_token_index];
        ERROR("Expected '(', but got: \"%s\"", t->input);
    }
    if (consume_next_token(')'))
        return params;

    vec_push(params, param());
    while (consume_next_token(',')) {
        vec_push(params, param());
    }
    expect_token(')');
    return params;
}

Node *stmt() {
    Node *node;
    if (consume_next_token(TK_RETURN)) {
        node = new_node(ND_RETURN, expr(), NULL);
        expect_token(';');
    }
    else if (consume_next_token(TK_IF)) {
        node = malloc(sizeof(Node));
        node->node_type = ND_IF;
        expect_token('(');
        node->condition = logical_or();
        expect_token(')');
        node->then = stmt();
        if (consume_next_token(TK_ELSE))
            node->els = stmt();
    }
    else if (consume_next_token('{')) {
        node = malloc(sizeof(Node));
        node->node_type = ND_BLOCK;
        node->stmts_in_block = new_vector();
        while (!consume_next_token('}'))
            vec_push(node->stmts_in_block, stmt());
    }
    else if (consume_next_token(TK_WHILE)) {
        node = malloc(sizeof(Node));
        node->node_type = ND_WHILE;
        expect_token('(');
        node->condition = logical_or();
        expect_token(')');
        node->body = stmt();
    }
    else if (consume_next_token(TK_FOR)) {
        node = malloc(sizeof(Node));
        node->node_type = ND_FOR;
        expect_token('(');
        node->init = stmt();
        node->condition = equality();
        expect_token(';');
        node->inc = assign();
        expect_token(')');
        node->body = stmt();
    }
    else if (check_next_token(TK_INT)) {
        node = decl_var();
        expect_token(';');
    }
    else {
        node = expr();
        expect_token(';');
    }
    return node;
}

Node *expr() {
    Node *node = assign();
    return node;
}

Node *assign() {
    Node *node = logical_or();
    if (consume_next_token('=')) {
        node = new_node('=', node, assign());
    }
    return node;
}

Node *logical_or() {
    Node *node = logical_and();
    for (;;) {
        if (consume_next_token(TK_OR))
            node = new_node(ND_OR, node, logical_and());
        else
            return node;
    }
}

Node *logical_and() {
    Node *node = equality();
    for (;;) {
        if (consume_next_token(TK_AND))
            node = new_node(ND_AND, node, equality());
        else
            return node;
    }
}

Node *equality() {
    Node *node = relational();
    for (;;) {
        if (consume_next_token(TK_EQ)) {
            node = new_node(ND_EQ, node, relational());
        }
        else if (consume_next_token(TK_NE)) {
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
        if (consume_next_token(TK_LE)) {
            node = new_node(ND_LE, node, add());
        }
        else if (consume_next_token('<')) {
            node = new_node('<', node, add());
        }
        else if (consume_next_token(TK_GE)) {
            node = new_node(ND_LE, add(), node);
        }
        else if (consume_next_token('>')) {
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
        if (consume_next_token('+'))
            node = new_node('+', node, mul());
        else if (consume_next_token('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();
    for (;;) {
        if (consume_next_token('*'))
            node = new_node('*', node, unary());
        else if (consume_next_token('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume_next_token('+'))
        return term();
    if (consume_next_token('-'))
        return new_node('-', new_node_num(0), term());
    if (consume_next_token('!'))
        return new_node('!', logical_or(), NULL);
    if (consume_next_token('&'))
        return new_node(ND_ADDR, unary(), NULL);
    if (consume_next_token('*'))
        return new_node(ND_DEREF, unary(), NULL);
    return term();
}

Node *term() {
    Token *t = g_tokens->data[g_token_index];
    if (consume_next_token('(')) {
        Node *node = expr();
        if (!consume_next_token(')'))
            ERROR("Expected ')', but got: \"%s\"", t->input);
        return node;
    }
    else if (t->type == TK_NUM) {
        g_token_index++;
        return new_node_num(t->value);
    }
    else if (t->type == TK_IDENT) {
        char *ident_name = t->name;
        g_token_index++;
        if (!consume_next_token('('))
            return new_node_var(ident_name);

        Vector *args = new_vector();
        if (consume_next_token(')'))
            return new_node_funccall(ident_name, args);

        vec_push(args, expr());
        while (consume_next_token(','))
            vec_push(args, expr());
        expect_token(')');
        return new_node_funccall(ident_name, args);
    }
    else if (t->type == TK_INT) {
        g_token_index++;
        return decl_var();
    }
    else {
        ERROR("Expected value, but got: \"%s\"", t->input);
        return NULL;
    }
}

Node *decl_var() {
    C_type *type = type_specifier();
    Token *t = g_tokens->data[g_token_index];
    if (t->type != TK_IDENT) {
        ERROR("Expected identifier name, but got \"%s\"", t->input);
        return NULL;
    }
    g_token_index++;

    map_set(g_var_type_map, t->name, (void *)type);
    Node *node = new_node_var(t->name);
    if (consume_next_token('='))
        node = new_node('=', node, assign());
    return node;
}