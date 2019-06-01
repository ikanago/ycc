#include "ycc.h"

void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

Vector *new_vector()
{
	Vector *v = malloc(sizeof(Vector));
	v->data = malloc(sizeof(void *) * 16);
	v->capacity = 16;
	v->len = 0;
	return v;
}

void vec_push(Vector *v, void *elem)
{
	if (v->capacity == v->len)
	{
		v->capacity *= 2;
		v->data = realloc(v->data, sizeof(void *) * v->capacity);
	}
	v->data[v->len++] = elem;
}

Map *new_map()
{
	Map *map = malloc(sizeof(Map));
	map->keys = new_vector();
	map->values = new_vector();
	return map;
}

void map_set(Map *map, char *key, void *value)
{
	vec_push(map->keys, key);
	vec_push(map->values, value);
}

void *map_get(Map *map, char *key)
{
	for (int i = map->keys->len - 1;i >= 0; i--)
	{
		if (!strcmp(map->keys->data[i], key))
			return map->values->data[i];
	}
	return NULL;
}

bool map_exists(Map *map, char *key)
{
	for (int i = 0; i < map->keys->len; i++)
	{
		if(!strcmp(map->keys->data[i], key))
			return true;
	}
	return false;
}