#include "9cc.h"

Token *add_token(Vector *v, int type, char *input)
{
	Token *t = malloc(sizeof(Token));
	t->type = type;
	t->input = input;
	vec_push(v, t);
	return t;
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
		else
		{
			fprintf(stderr, "Cannot tokenize: %s\n", p);
			exit(1);
		}
	}
	add_token(v, TK_EOF, p);
	return v;
}
