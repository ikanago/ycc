#include "ycc.h"

int main(int argc, char **argv) {
    if (argc == 2 && !strcmp(argv[1], "--test")) {
        vec_test();
        map_test();
        stringbuilder_test();
        return 0;
    }

    if (argc == 3 && !strcmp(argv[1], "--dump-token")) {
        Vector *tokens = tokenize(argv[2]);
        dump_token(tokens);
        return 0;
    }

    if (argc == 3 && !strcmp(argv[1], "--dump-node")) {
        Vector *tokens = tokenize(argv[2]);
        Vector *nodes = parse(tokens);
        dump_nodes(nodes);
        return 0;
    }

    Vector *tokens = tokenize(argv[1]);
    Vector *nodes = parse(tokens);
    codegen(nodes);

    return 0;
}
