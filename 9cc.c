#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	TK_NUM = 256,
	TK_EOF,
};

typedef struct {
	int type;
	int value;
	char *input;
} Token;

Token tokens[100];

void tokenize(char *p) {
	int i = 0;
	while (*p) {
		if(isspace(*p)) {
			p++;
		}
		else if (*p == '+' || *p == '-') {
			tokens[i].type = *p;
			tokens[i].input = p;
			i++;
			p++;
		}
		else if (isdigit(*p)) {
			tokens[i].type = TK_NUM;
			tokens[i].input = p;
			tokens[i].value = strtol(p, &p, 10);
			i++;
		}
		else {
			fprintf(stderr, "Cannot tokenize: %s\n", p);
			exit(1);
		}
	}
	tokens[i].type = TK_EOF;
	tokens[i].input = p;
}

void error(int i) {
	fprintf(stderr, "Unexpected token: %s\n", tokens[i].input);
	exit(1);
}

int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "The number of arguments is incorrect.\n");
		return 1;
	}

	tokenize(argv[1]);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	if (tokens[0].type != TK_NUM) {
		error(0);
	}
	printf("  mov rax, %d\n", tokens[0].value);

	int i = 1;
	while (tokens[i].type != TK_EOF) {
		if (tokens[i].type == '+') {
			i++;
	        if (tokens[i].type != TK_NUM) {
		        error(i);
	        }
			printf("  add rax, %d\n", tokens[i].value);
			i++;
		}
		else if (tokens[i].type == '-') {
			i++;
	        if (tokens[i].type != TK_NUM) {
		        error(i);
	        }
			printf("  sub rax, %d\n", tokens[i].value);
			i++;
		}
		else {
			error(i);
		}
	}
	printf("  ret\n");
	return 0;
}
