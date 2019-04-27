#include "9cc.h"

int expect(int line, int expected, int actual) {
	if (expected == actual) {
		printf("test passed\n");
		return 0;
	}
	fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
	exit(1);
}

void vec_test() {
	Vector *v = new_vector();
	expect(__LINE__, 0, v->len);

	for (int i = 0; i < 100; i++) {
		vec_push(v, (void *)(intptr_t)i);
	}

	expect(__LINE__, 100, v->len);
	expect(__LINE__, 0, (intptr_t)v->data[0]);
	expect(__LINE__, 50, (intptr_t)v->data[50]);
	expect(__LINE__, 99, (intptr_t)v->data[99]);
	printf("%ld\n", sizeof(v->data[0]));

	printf("OK\n");
}

