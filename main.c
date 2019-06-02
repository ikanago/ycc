#include "ycc.h"
#ifdef DEBUG
bool is_debug = true;
#else
bool is_debug = false;
#endif

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

	Map *variable_map = new_map();
	Vector *tokens = tokenize(argv[1]);
	Vector *nodes = parse(tokens, variable_map);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, %d\n", variable_offset);
	debug_printf("; prologue\n");

	codegen(nodes, variable_map);

	printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");
	debug_printf("; end of a program\n");
	return 0;
}
