#include "ycc.h"

Map *g_keywords;
Map *g_string_map;
int string_literal_label;

Token *add_token(Vector *v, int type, char *input) {
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->input = input;
    vec_push(v, t);
    return t;
}

bool is_new_line(const char *c) { return *c == '\\' && *(c + 1) == 'n'; }

bool is_digit(char c) { return '0' <= c && c <= '9'; }

bool is_alnum(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') || (c == '_');
}

Map *reserve_keywords() {
    Map *keywords = new_map();
    map_set(keywords, "if", (void *)TK_IF);
    map_set(keywords, "else", (void *)TK_ELSE);
    map_set(keywords, "while", (void *)TK_WHILE);
    map_set(keywords, "for", (void *)TK_FOR);
    map_set(keywords, "return", (void *)TK_RETURN);
    map_set(keywords, "sizeof", (void *)TK_SIZEOF);
    map_set(keywords, "int", (void *)TK_INT);
    map_set(keywords, "char", (void *)TK_CHAR);
    return keywords;
}

Vector *scan(char *pos) {
    Vector *v = new_vector();
    while (*pos) {
        if (isspace(*pos)) {
            pos++;
        }
        else if (is_new_line(pos)) {
            pos += 2;
        }
        else if (!strncmp(pos, "//", 2)) {
            pos += 2;
            while (*pos != '\n')
                pos++;
        }
        else if (!strncmp(pos, "/*", 2)) {
            pos += 2;
            char *comment_end = strstr(pos, "*/");
            if (!comment_end)
                ERROR("Unclosed block comment: \"%s\"", pos);
            pos = comment_end + 2;
        }
        else if (is_digit(*pos)) {
            Token *t = add_token(v, TK_NUM, pos);
            t->type_name = "INT";
            t->value =
                strtol(pos, &pos, 10); // this function increments pointer
        }
        else if (!strncmp(pos, "+=", 2)) {
            Token *t = add_token(v, TK_ADD_ASSIGN, pos);
            t->type_name = "ADD_ASSIGN";
            t->name = "+=";
            pos += 2;
        }
        else if (!strncmp(pos, "-=", 2)) {
            Token *t = add_token(v, TK_SUB_ASSIGN, pos);
            t->type_name = "SUB_ASSIGN";
            t->name = "-=";
            pos += 2;
        }
        else if (!strncmp(pos, "*=", 2)) {
            Token *t = add_token(v, TK_MUL_ASSIGN, pos);
            t->type_name = "MUL_ASSIGN";
            t->name = "*=";
            pos += 2;
        }
        else if (!strncmp(pos, "/=", 2)) {
            Token *t = add_token(v, TK_DIV_ASSIGN, pos);
            t->type_name = "DIV_ASSIGN";
            t->name = "/=";
            pos += 2;
        }
        else if (!strncmp(pos, "==", 2)) {
            Token *t = add_token(v, TK_EQ, pos);
            t->type_name = "EQUAL";
            t->name = "==";
            pos += 2;
        }
        else if (!strncmp(pos, "!=", 2)) {
            Token *t = add_token(v, TK_NE, pos);
            t->type_name = "NOT_EQUAL";
            t->name = "!=";
            pos += 2;
        }
        else if (!strncmp(pos, "<=", 2)) {
            Token *t = add_token(v, TK_LE, pos);
            t->type_name = "LESS/EQUAL";
            t->name = "<=";
            pos += 2;
        }
        else if (!strncmp(pos, ">=", 2)) {
            Token *t = add_token(v, TK_GE, pos);
            t->type_name = "GREATER/EQUAL";
            t->name = ">=";
            pos += 2;
        }
        else if (!strncmp(pos, "&&", 2)) {
            Token *t = add_token(v, TK_AND, pos);
            t->type_name = "AND";
            t->name = "&&";
            pos += 2;
        }
        else if (!strncmp(pos, "||", 2)) {
            Token *t = add_token(v, TK_OR, pos);
            t->type_name = "OR";
            t->name = "||";
            pos += 2;
        }
        else if (!strncmp(pos, "++", 2)) {
            Token *t = add_token(v, TK_INCR, pos);
            t->type_name = "INCREMENT";
            t->name = "++";
            pos += 2;
        }
        else if (!strncmp(pos, "--", 2)) {
            Token *t = add_token(v, TK_DECR, pos);
            t->type_name = "DECREMENT";
            t->name = "--";
            pos += 2;
        }
        else if (isalpha(*pos) || *pos == '_') {
            int length = 1;
            while (is_alnum(pos[length]))
                length++;

            char *name = strndup(pos, length);
            int type = (intptr_t)map_get(g_keywords, name);
            if (!type)
                type = TK_IDENT;
            Token *t = add_token(v, type, pos);
            t->type_name = "IDENTIFIER";
            t->name = name;
            pos += length;
        }
        else if (strchr("+-*/;=(){}<>,!&[]", *pos)) {
            Token *t = add_token(v, *pos, pos);
            t->type_name = strndup(pos, 1);
            t->name = strndup(pos, 1);
            pos++;
        }
        else if (strchr("\"", *pos)) {
            pos++;
            int length = 0;
            while (!strchr("\"", pos[length]))
                length++;

            char *string_literal = strndup(pos, length);
            map_set(g_string_map, string_literal, (void *)(intptr_t)string_literal_label++);
            Token *t = add_token(v, TK_STRING, pos);
            t->type_name = "STRING";
            t->name = string_literal;
            pos += length + 1;
        }
        else {
            ERROR("Cannot tokenize: \"%s\"", pos);
        }
    }
    add_token(v, TK_EOF, pos);
    return v;
}

Vector *tokenize(char *pos) {
    g_keywords = reserve_keywords();
    g_string_map = new_map();
    string_literal_label = 0;
    return scan(pos);
}