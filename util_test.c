#include "ycc.h"

int examine(int line, int examineed, int actual) {
    if (examineed == actual) {
        printf("test passed\n");
        return 0;
    }
    fprintf(stderr, "%d: %d examineed, but got %d\n", line, examineed, actual);
    exit(1);
}

void vec_test() {
    printf("---Vector test---\n");
    Vector *v = new_vector();
    examine(__LINE__, 0, v->len);

    for (int i = 0; i < 100; i++) {
        vec_push(v, (void *)(intptr_t)i);
    }

    examine(__LINE__, 100, v->len);
    examine(__LINE__, 0, (intptr_t)v->data[0]);
    examine(__LINE__, 50, (intptr_t)v->data[50]);
    examine(__LINE__, 99, (intptr_t)v->data[99]);
    printf("%ld\n", sizeof(v->data[0]));

    printf("OK\n");
}

void map_test() {
    printf("---Map test---\n");
    Map *map = new_map();
    examine(__LINE__, 0, (intptr_t)map_get(map, "foo"));

    map_set(map, "foo", (void *)2);
    examine(__LINE__, 2, (intptr_t)map_get(map, "foo"));

    map_set(map, "bar", (void *)1);
    examine(__LINE__, 1, (intptr_t)map_get(map, "bar"));

    map_set(map, "foo", (void *)6);
    examine(__LINE__, 6, (intptr_t)map_get(map, "foo"));

    examine(__LINE__, true, map_exists(map, "foo"));
    examine(__LINE__, false, map_exists(map, "baz"));
}