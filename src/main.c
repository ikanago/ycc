#include "ycc.h"

char *read_file(char *file_path) {
    FILE *source = fopen(file_path, "r");
    if (source == NULL) {
        perror(file_path);
        exit(1);
    }

    StringBuilder *code = new_stringbiulder();
    int n = 256;
    char buffer[n];
    while (fgets(buffer, n, source) != NULL) {
        stringbuilder_append(code, buffer);
    }
    return code->entity;
}

int main(int argc, char **argv) {
    if (argc == 2 && !strcmp(argv[1], "--test")) {
        vec_test();
        map_test();
        stringbuilder_test();
        return 0;
    }

    bool is_dump_token = false;
    bool is_dump_nodes = false;
    bool is_raw = false;
    char *code;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--dump-token"))
            is_dump_token = true;
        else if (!strcmp(argv[i], "--dump-node"))
            is_dump_nodes = true;
        else if (!strcmp(argv[i], "--raw")) {
            code = argv[argc - 1];
            is_raw = true;
        }
    }

    if (!is_raw)
        code = read_file(argv[argc - 1]);

    Vector *tokens = tokenize(code);
    Vector *nodes = parse(tokens);
    if (is_dump_token)
        dump_token(tokens);
    if (is_dump_nodes)
        dump_nodes(nodes);
    if (!(is_dump_token || is_dump_nodes))
        codegen(nodes);

    return 0;
}
