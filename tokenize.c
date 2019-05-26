#include "9cc.h"

Token *add_token(Vector *v, int type, char *input)
{
	Token *t = malloc(sizeof(Token));
	t->type = type;
	t->input = input;
	vec_push(v, t);
	return t;
}

int is_alnum(char c)
{
	return ('a' <= c && c <= 'z') ||
		   ('A' <= c && c <= 'Z') ||
		   ('0' <= c && c <= '9') ||
		   (c == '_');
}

Vector *tokenize(char *p)
{
	Vector *v = new_vector();
	while (*p)
	{
		if (isspace(*p))
		{
			p++;
		}
		else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
				 *p == ')')
		{
			add_token(v, *p, p);
			p++;
		}
		else if (isdigit(*p))
		{
			Token *t = add_token(v, TK_NUM, p);
			t->value = strtol(p, &p, 10);
		}
		else if (!strncmp(p, "==", 2))
		{
			add_token(v, TK_EQ, p);
			p += 2;
		}
		else if (!strncmp(p, "!=", 2))
		{
			add_token(v, TK_NE, p);
			p += 2;
		}
		else if (!strncmp(p, "<=", 2))
		{
			add_token(v, TK_LE, p);
			p += 2;
		}
		else if (*p == '<')
		{
			add_token(v, '<', p);
			p++;
		}
		else if (!strncmp(p, ">=", 2))
		{
			add_token(v, TK_GE, p);
			p += 2;
		}
		else if (*p == '>')
		{
			add_token(v, '>', p);
			p++;
		}
		else if (!strncmp(p, "return", 6) && !is_alnum(p[6]))
		{
			add_token(v, TK_RETURN, p);
			p += 6;
		}
		else if ('a' <= *p && *p <= 'z')
		{
			Token *t = add_token(v, TK_IDENT, p);
			t->name = *p;
			p++;
		}
		else if (*p == '=')
		{
			add_token(v, '=', p);
			p++;
		}
		else if (*p == ';')
		{
			add_token(v, ';', p);
			p++;
		}
		else
		{
			fprintf(stderr, "Cannot tokenize: %s\n", p);
			exit(1);
		}
	}
	add_token(v, TK_EOF, p);
	return v;
}
