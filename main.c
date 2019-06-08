#include "ycc.h"
#ifdef DEBUG
bool is_debug = true;
#else
bool is_debug = false;
#endif

int main(int argc, char **argv) {
    if (argc == 2 && !strcmp(argv[1], "--test")) {
        vec_test();
        map_test();
        return 0;
    }

    if (argc == 3 && !strcmp(argv[1], "--dump-token")) {
        Vector *tokens = tokenize(argv[2]);
        dump_token(tokens);
        return 0;
    }

    Vector *tokens = tokenize(argv[1]);
    Vector *nodes = parse(tokens);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", variable_offset);
    debug_printf("; prologue\n");

    codegen(nodes);

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    debug_printf("; end of a program\n");
    return 0;
}
