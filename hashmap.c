#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "hashmap.h"

#define DEFAULT_CAP 7

struct DictEntry {
	int key;
	int value;
	unsigned long hash;
};

struct HashMap {
	struct DictEntry **table;
	size_t size;
	size_t capacity;
};

static struct DictEntry* get_entry(HashMap *map, int key);
static void destroy_table(struct DictEntry **e, size_t size);
static inline unsigned long inthash (int num);
static inline size_t wrapped_inc(size_t n, size_t capacity);

static bool is_prime(int n)
{
	if (n % 2 == 0)
		return n == 2;
	if (n % 3 == 0)
		return n == 3;

	int k = 4;
	int limit = sqrt(n) + 1;
	for (int i=5; i <= limit; k = 6 - k, i+= k) {
		if (n % i == 0)
			return false;
	}
	return true;
}

static int next_prime(int n)
{
	while (!is_prime(++n));
	return n;
}

/* Create an empty hashmap
 * Returns a pointer to the map on success,
 * NULL on failure */
HashMap* new_hashmap(void)
{
	HashMap *new = malloc(sizeof(HashMap));
	if (!new) {
		goto ERR;
	}

	new->size = 0;
	new->capacity = DEFAULT_CAP;
	new->table = malloc(new->capacity * sizeof(struct DictEntry *));
	if (!new->table) {
		goto ERR;
	}

	for (size_t i=0; i<new->capacity; i++) {
		new->table[i] = NULL;
	}
	return new;

ERR:
	if (new) {
		free(new);
	}
	return NULL;
}

void destroy_hashmap(HashMap *map)
{
	destroy_table(map->table, map->capacity);
	free(map);
}

/* TODO: If the hashmap is full (somehow) this will never return */
/* Return the index of the next unoccupied slot in the table for element elem */
static size_t get_index(const HashMap *map, int elem)
{
	size_t index = inthash(elem) % map->capacity;
	while (map->table[index]) {
		index = wrapped_inc(index, map->capacity);
	}
	return index;
}

/* Increase the size of the table to the next prime number
 * that is >= twice the previous size. Rehashes every element
 * 
 * Returns nonzero on failure */
static int hashmap_extend(HashMap *map)
{
	size_t old_capacity = map->capacity;
	size_t new_capacity = next_prime(2 * map->capacity);

	struct DictEntry **old_table = map->table;
	struct DictEntry **new_table = malloc(new_capacity * sizeof(struct DictElem *));
	if (!new_table) {
		return 1;
	}

	map->table = new_table;
	map->capacity = new_capacity;
	/* NULL-out the new table */
	for (size_t i=0; i<map->capacity; map->table[i] = NULL, i++);

	/* Rehash elements into the new table */
	for (size_t i=0; i<old_capacity; i++) {
		struct DictEntry *elem = old_table[i];
		if (!elem) {
			continue;
		}

		size_t index = get_index(map, elem->key);
		map->table[index] = elem;
	}
	free(old_table);
	return 0;
}

/* Keep the table working with linear probing after deletion */
void rectify(HashMap *map, size_t index)
{
	size_t cursor = wrapped_inc(index, map->capacity);
	while (map->table[cursor]) {
		struct DictEntry *elem = map->table[cursor];
		map->table[cursor] = NULL;

		size_t index = get_index(map, elem->key);
		map->table[index] = elem;
		cursor = wrapped_inc(cursor, map->capacity);
	}
}

/* TODO: This could maybe make use of elem->hash */
static struct DictEntry *get_entry(HashMap *map, int key)
{
	size_t index = inthash(key) % map->capacity;
	while (map->table[index]) {
		if (key == map->table[index]->key) {
			return map->table[index];
		}
		index = wrapped_inc(index, map->capacity);
	}
	return NULL;
}

/* Insert key:value pair into the hashmap
 * If key already exists, value is overwritten
 *
 * Returns non-zero on failure */
int hashmap_insert(HashMap *map, int key, int value)
{
	/* Resize if the map is getting too cluttered */
	if (map->size > 2*(map->capacity / 3)) {
		if (hashmap_extend(map)) {
			return 1;
		}
	}

	struct DictEntry *e = get_entry(map, key);
	if (e) {
		e->value = value;
	} else {
		struct DictEntry *new = malloc(sizeof(struct DictEntry));
		if (!new) {
			return 1;
		}
		new->hash = inthash(key);
		new->key = key;
		new->value = value;

		size_t index = get_index(map, key);
		map->table[index] = new;
		map->size++;
	}
	return 0;
}

/* Get the element indexed by key, and place it in result 
 * Returns non-zero and leaves result unchanged on failure */
int hashmap_get(HashMap *map, int key, int *result)
{
	struct DictEntry *entry = get_entry(map, key);
	if (entry) {
		*result = entry->value;
		return 0;
	}
	return 1;
}

/* Remove key:value from the map
 *
 * Returns non-zero if key does not exist in map */
int hashmap_remove(HashMap *map, int key)
{
	size_t index = inthash(key) % map->capacity;
	while (map->table[index]) {
		if (key == map->table[index]->key) {
			free(map->table[index]);
			map->table[index] = NULL;
			rectify(map, index);
			return 0;
		}
		index = wrapped_inc(index, map->capacity);
	}
	return 1;
}

static void destroy_table(struct DictEntry **e, size_t size) {
	for (size_t i=0; i<size; i++) {
		free(e[i]);
	}
	free(e);
}

static inline unsigned long inthash (int num)
{
	return (unsigned long) num * 2654435761;
}

static inline size_t wrapped_inc(size_t n, size_t capacity)
{
	return (n + 1) % capacity;
}
