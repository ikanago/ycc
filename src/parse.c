#include "ycc.h"

Vector *g_tokens;       // Token array received from tokenizer.
Vector *g_nodes;        // Node array to store parsed node.
int g_token_index;      // Index to indicate current position at `g_tokens`.
Map *g_lvar_offset_map; // Map to store sets of local variable and stack offset.
int g_variable_offset;  // Sum of variable's offset.
Map *g_lvar_type_map;   // Map to store sets of local variable and its type.
Map *g_gvar_type_map;   // Map to store sets of global variable and its type.
Vector *g_gvar_nodes;   // Node array to store nodes of global variables.

Token *get_token(int index) {
    Token *t = g_tokens->data[index];
    return t;
}

// Check if current token's type is expected one.
void expect_token(int type) {
    Token *t = get_token(g_token_index);
    if (t->type != type)
        ERROR("%c (%d) expected, but got %c (%d) at \"%s\"", type, type,
              t->type, t->type, t->input);
    g_token_index++;
}

// Check if current token's type is desired one.
// If so, proceed to next token.
int consume_next_token(int type) {
    Token *t = get_token(g_token_index);
    if (t->type != type)
        return 0;
    g_token_index++;
    return 1;
}

// Check if current token's type is desired one.
// Never proceed to next token.
int check_next_token(int type) {
    if (get_token(g_token_index)->type != type) {
        return 0;
    }
    return 1;
}

// Generate new C_type struct following information passed by arguments.
C_type *new_type(int TY_type, int size, C_type *ptr_to) {
    C_type *type = malloc(sizeof(C_type));
    type->type = TY_type;
    type->size = size;
    type->ptr_to = ptr_to;
    while (consume_next_token('*'))
        type = new_type(TY_PTR, 8, type);
    return type;
}

C_type *type_specifier() {
    Token *t = get_token(g_token_index);
    if (consume_next_token(TK_INT)) {
        return new_type(TY_INT, 4, NULL);
    }
    ERROR("Expected type name, but got \"%s\"", t->input);
    return NULL;
}

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

Node *new_node_funccall(char *name, Vector *args) {
    Node *node = malloc(sizeof(Node));
    node->node_type = ND_FUNCCALL;
    node->name = name;
    node->args = args;
    return node;
}

Node *new_node_def_func(C_type *type, char *name, Vector *params, Node *body) {
    Node *node = malloc(sizeof(Node));
    node->node_type = ND_DEF_FUNC;
    node->name = name;
    node->c_type = type;
    node->params = params;
    node->body = body;
    return node;
}

// This function is
Node *decl_var(C_type *var_type, char *name) {
    if (consume_next_token('[')) {
        var_type->array_of = new_type(var_type->type, 4, NULL);
        var_type->type = TY_ARRAY;
        var_type->array_len = get_token(g_token_index)->value;
        g_token_index++;
        expect_token(']');
    }
    Node *node = malloc(sizeof(Node));
    node->name = name;
    node->c_type = var_type;
    return node;
}

// A type and name of global variable is determined before this function is called.
Node *new_node_decl_gvar(C_type *var_type, char *name) {
    Node *node = decl_var(var_type, name);
    node->node_type = ND_GVAR;

    map_set(g_gvar_type_map, name, node->c_type);
    if (consume_next_token('=')) {
        return new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *new_node_decl_lvar() {
    // Determine a type of a local variable.
    C_type *var_type = type_specifier();
    Token *t = get_token(g_token_index);
    if (get_token(g_token_index)->type != TK_IDENT) {
        ERROR("Expected identifier name, but got \"%s\"", t->input);
        return NULL;
    }

    char *name = get_token(g_token_index)->name;
    g_token_index++;

    Node *node = decl_var(var_type, name);
    node->node_type = ND_LVAR;
    // Array variable
    if (node->c_type->type == TY_ARRAY) {
        node->c_type->array_size = node->c_type->size * node->c_type->array_len;
        g_variable_offset += node->c_type->array_size;
    }
    // Variable
    else
        g_variable_offset += node->c_type->size;
    map_set(g_lvar_type_map, name, (void *)var_type);
    map_set(g_lvar_offset_map, name, (void *)(intptr_t)g_variable_offset);
    if (consume_next_token('=')) {
        return new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *new_node_var(char *name) {
    Node *node = malloc(sizeof(Node));
    node->name = name;
    node->c_type = (C_type *)map_get(g_lvar_type_map, name);

    if (node->c_type != NULL)
        node->node_type = ND_LVAR;
    else if (node->c_type == NULL) {
        node->c_type = (C_type *)map_get(g_gvar_type_map, name);
        node->node_type = ND_GVAR;
    }
    return node;
}

// Initialize parser and start parsing.
Vector *parse(Vector *v) {
    g_tokens = v;
    g_nodes = new_vector();
    g_token_index = 0;
    g_gvar_type_map = new_map();
    g_gvar_nodes = new_vector();
    return program();
}

// Entry point of constructing syntax tree.
// Top node of each tree will be function type node.
Vector *program() {
    for (;;) {
        if (get_token(g_token_index)->type == TK_EOF)
            break;

        C_type *var_type = type_specifier();
        Token *t = get_token(g_token_index);
        char *ident_name = t->name;
        if (t->type != TK_IDENT)
            ERROR("Expected identifier name, but got \"%s\"", t->input);
        g_token_index++;

        // Function definition
        if (consume_next_token('(')) {
            g_lvar_offset_map = new_map();
            g_lvar_type_map = new_map();
            g_variable_offset = 0;
            Node *node = define_func(var_type, ident_name);
            node->lvar_offset_map = g_lvar_offset_map;
            node->max_variable_offset = g_variable_offset;
            vec_push(g_nodes, node);
        }
        // Global variable
        else {
            Node *node = new_node_decl_gvar(var_type, ident_name);
            vec_push(g_gvar_nodes, node);
            expect_token(';');
        }
    }
    vec_push(g_nodes, NULL);
    return g_nodes;
}

Node *define_func(C_type *func_type, char *func_name) {
    Vector *params = func_params();
    Node *body = stmt();
    Node *node = new_node_def_func(func_type, func_name, params, body);
    return node;
}

Vector *func_params() {
    Vector *params = new_vector();
    if (consume_next_token(')'))
        return params;

    vec_push(params, new_node_decl_lvar());
    while (consume_next_token(',')) {
        vec_push(params, new_node_decl_lvar());
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
        node = new_node_decl_lvar();
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
        node = new_node(ND_ASSIGN, node, assign());
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
            node = new_node(ND_LESS, node, add());
        }
        else if (consume_next_token(TK_GE)) {
            node = new_node(ND_LE, add(), node);
        }
        else if (consume_next_token('>')) {
            node = new_node(ND_LESS, add(), node);
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
            node = new_node(ND_ADD, node, mul());
        else if (consume_next_token('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();
    for (;;) {
        if (consume_next_token('*'))
            node = new_node(ND_MUL, node, unary());
        else if (consume_next_token('/'))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume_next_token('+'))
        return term();
    if (consume_next_token('-'))
        return new_node(ND_SUB, new_node_num(0), term());
    if (consume_next_token('!'))
        return new_node(ND_NOT, logical_or(), NULL);
    if (consume_next_token('&'))
        return new_node(ND_ADDR, unary(), NULL);
    if (consume_next_token('*'))
        return new_node(ND_DEREF, unary(), NULL);
    if (consume_next_token(TK_SIZEOF))
        return new_node(ND_SIZEOF, unary(), NULL);
    return term();
}

Node *term() {
    Token *t = get_token(g_token_index);
    if (consume_next_token('(')) {
        Node *node = expr();
        if (!consume_next_token(')'))
            ERROR("Expected ')', but got: \"%s\"", t->input);
        return node;
    }
    // Integer literal
    else if (t->type == TK_NUM) {
        g_token_index++;
        return new_node_num(t->value);
    }
    else if (t->type == TK_IDENT) {
        char *ident_name = t->name;
        g_token_index++;
        // Function call
        if (consume_next_token('(')) {
            Vector *args = new_vector();
            // Function call without argument
            if (consume_next_token(')'))
                return new_node_funccall(ident_name, args);

            vec_push(args, expr());
            while (consume_next_token(','))
                vec_push(args, expr());
            expect_token(')');
            return new_node_funccall(ident_name, args);
        }
        else if (consume_next_token('[')) {
            Node *array_index_node = assign();
            expect_token(']');
            Node *node = new_node(ND_ADD, new_node_var(ident_name), array_index_node);
            return new_node(ND_DEREF, node, NULL);
        }
        // Local Variable
        else {
            return new_node_var(ident_name);
        }
    }
    // Token "int"
    else if (t->type == TK_INT) {
        g_token_index++;
        return new_node_decl_lvar();
    }
    else {
        ERROR("Expected value, but got: \"%s\"", t->input);
        return NULL;
    }
}