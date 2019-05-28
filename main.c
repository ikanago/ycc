#include "ycc.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "The number of arguments is incorrect.\n");
		return 1;
	}

	if (argc == 2 && !strcmp(argv[1], "-test"))
	{
		vec_test();
		map_test();
		return 0;
	}

	Vector *tokens = tokenize(argv[1]);
	Vector *nodes = parse(tokens);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, 208\n");
	printf("; prologue\n");

	for (int i = 0; nodes->data[i]; i++)
	{
		gen(nodes->data[i]);
		printf("  pop rax\n");
		printf("; end of a statement\n\n");
	}

    printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");
	printf("; end of a program\n");
	return 0;
}
