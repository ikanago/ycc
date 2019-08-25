#include "ycc.h"

void usage() {
    printf("Usage: ycc [options] <file|code>\n");
    printf("Options:\n");
    printf(" --help          Show compiler usage.\n");
    printf(" --test          Run test to check each compiler works well.\n");
    printf(
        " --raw           Receive source code as a command line argument.\n");
    printf(" --dump-token    Display analyzed token.\n");
    printf(" --dump-node     Display tree structures of analyzed node.\n");
}

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
        stringbuilder_append(
            code, buffer); // `buffer` is overwritten every `fgets` execution.
    }
    return code->entity;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        if (!strcmp(argv[1], "--test")) {
            util_test();
            return 0;
        }
        else if (!strcmp(argv[1], "--help")) {
            usage();
            return 0;
        }
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
    if (is_dump_token) {
        dump_token(tokens);
    }
    if (is_dump_nodes) {
        Vector *nodes = parse(tokens);
        dump_nodes(nodes);
    }
    if (!(is_dump_token || is_dump_nodes)) {
        Vector *nodes = parse(tokens);
        sema_parse_nodes(nodes);
        codegen(nodes);
    }
    return 0;
}
