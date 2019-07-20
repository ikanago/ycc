#include "ycc.h"

Map *g_keywords;

Token *add_token(Vector *v, int type, char *input) {
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->input = input;
    vec_push(v, t);
    return t;
}

bool is_new_line(char *c) { return *c == '\\' && *(c + 1) == 'n'; }

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
        else if (is_digit(*pos)) {
            Token *t = add_token(v, TK_NUM, pos);
            t->type_name = "INT";
            t->value =
                strtol(pos, &pos, 10); // this function increments pointer
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
        else if (isalpha(*pos)) {
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
        else if (strchr("+-*/;=(){}<>,", *pos)) {
            Token *t = add_token(v, *pos, pos);
            t->type_name = strndup(pos, 1);
            t->name = strndup(pos, 1);
            pos++;
        }
        else {
            ERROR("Cannot tokenize: %s", pos);
        }
    }
    add_token(v, TK_EOF, pos);
    return v;
}

Vector *tokenize(char *pos) {
    g_keywords = reserve_keywords();
    return scan(pos);
}