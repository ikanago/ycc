#include "ycc.h"

int examine(int line, int examined, int actual) {
    if (examined == actual) {
        printf("test passed\n");
        return 0;
    }
    fprintf(stderr, "%d: %d examined, but got %d\n", line, examined, actual);
    exit(1);
}

int examine_string(int line, char *examined, char *actual) {
    if (strcmp(examined, actual) == 0) {
        printf("test passed\n");
        return 0;
    }
    fprintf(stderr, "%d: %s examined, but got %s\n", line, examined, actual);
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

void stringbuilder_test() {
    printf("---StringBuilder test---\n");
    StringBuilder *sb = new_stringbiulder();
    examine(__LINE__, 0, sb->len);

    stringbuilder_append(sb, "hoge");
    examine_string(__LINE__, "hoge", sb->entity);
    examine(__LINE__, 4, sb->len);

    stringbuilder_append(sb, "fuga");
    examine_string(__LINE__, "hogefuga", sb->entity);
    examine(__LINE__, 8, sb->len);

    char *str = (char *)malloc(sizeof(char) * 42);
    for (int i = 0; i < 42; i++) {
        str[i] = 'A';
    }
    stringbuilder_append(sb, str);
    examine_string(__LINE__,
                   "hogefugaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                   sb->entity);
    examine(__LINE__, 50, sb->len);
}

void util_test() {
    vec_test();
    map_test();
    stringbuilder_test();
}