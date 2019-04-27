#include "9cc.h"

Vector *new_vector() {
	Vector *v = malloc(sizeof(Vector));
	v->data = malloc(sizeof(void *) * 16);
	v->capacity = 16;
	v->len = 0;
	return v;
}

void vec_push(Vector *v, void *elem) {
	if (v->capacity == v->len) {
		v->capacity *= 2;
		v->data = realloc(v->data, sizeof(void *) * v->capacity);
	}
	v->data[v->len++] = elem;
}

