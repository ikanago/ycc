#include "9cc.h"

int expect(int line, int expected, int actual)
{
	if (expected == actual)
	{
		printf("test passed\n");
		return 0;
	}
	fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
	exit(1);
}

void vec_test()
{
	printf("---Vector test---\n");
	Vector *v = new_vector();
	expect(__LINE__, 0, v->len);

	for (int i = 0; i < 100; i++)
	{
		vec_push(v, (void *)(intptr_t)i);
	}

	expect(__LINE__, 100, v->len);
	expect(__LINE__, 0, (intptr_t)v->data[0]);
	expect(__LINE__, 50, (intptr_t)v->data[50]);
	expect(__LINE__, 99, (intptr_t)v->data[99]);
	printf("%ld\n", sizeof(v->data[0]));

	printf("OK\n");
}

void map_test()
{
	printf("---Map test---\n");
	Map *map = new_map();
	expect(__LINE__, 0, (long)map_get(map, "foo"));

	map_set(map, "foo", (void *)2);
	expect(__LINE__, 2, (long)map_get(map, "foo"));

	map_set(map, "bar", (void *)1);
	expect(__LINE__, 1, (long)map_get(map, "bar"));

	map_set(map, "foo", (void *)6);
	expect(__LINE__, 6, (long)map_get(map, "foo"));
}