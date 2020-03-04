#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hashmap.h"

#define DEFAULT_CAP 7
#define PRIME 37

struct DictEntry {
	void *key;
	void *value;
	unsigned long hash;
};

typedef struct HashMap {
	struct DictEntry **table;
	size_t size;
	size_t capacity;
	struct KeyOps kops;
	struct ValOps vops;
} HashMap;

int hashmap_insert(HashMap *set, void *key, void *value);
static struct DictEntry* get_entry(HashMap *set, void *key);

int is_prime(int n)
{
	if (n % 2 == 0)
		return n == 2;
	if (n % 3 == 0)
		return n == 3;

	int k = 4;
	int limit = sqrt(n) + 1;
	for (int i=5; i <= limit; k = 6 - k, i+= k) {
		if (n % i == 0)
			return 0;
	}
	return 1;
}

int next_prime(int n)
{
	while (!is_prime(++n));
	return n;
}

HashMap* new_hashmap(struct KeyOps kops, struct ValOps vops)
{
	HashMap *new = malloc(sizeof(HashMap));
	if (!new)
		goto ERR;
	new->capacity = DEFAULT_CAP;
	new->table = malloc(new->capacity * sizeof(struct DictEntry *));
	if (!new->table)
		goto ERR;
	new->kops = kops;
	new->vops = vops;
	new->size = 0;
	for (int i=0; i<new->capacity; i++)
		new->table[i] = NULL;
	return new;

ERR:
	if (new)
		free(new);
	return NULL;
}

static void destroy_table(HashMap *set)
{
	for (int i=0; i<set->capacity; i++) {
		if (set->table[i]) {
			set->kops.destroy(set->table[i]->key, set->kops.arg);
			set->vops.destroy(set->table[i]->value, set->vops.arg);
		}
		free(set->table[i]);
	}
	free(set->table);
}

void destroy_hashmap(HashMap *set)
{
	destroy_table(set);
	free(set);
}

static size_t get_index(const HashMap *set, const void *elem)
{
	size_t index = set->kops.hash(elem, set->kops.arg) % set->capacity;
	while (set->table[index])
		index = (index + 1) % set->capacity;
	return index;
}

static int hashmap_extend(HashMap *set)
{
	size_t old_capacity = set->capacity;
	size_t new_capacity = next_prime(set->capacity * 2);

	struct DictEntry **old_table = set->table;
	struct DictEntry **new_table = malloc(new_capacity * sizeof(void*));
	if (!new)
		return 1;
	set->table = new_table;
	set->capacity = new_capacity;
	for (int i=0; i<set->capacity; i++) set->table[i] = NULL;

	for (int i=0; i<old_capacity; i++) {
		struct DictEntry *elem = old_table[i];
		if (!elem)
			continue;
		size_t index = get_index(set, elem->key);
		set->table[index] = elem;
	}
	free(old_table);
	return 0;
}

// Keep the table working with linear probing after deletion
void rectify(HashMap *set, size_t index)
{
	size_t cursor = index + 1;
	while (set->table[cursor]) {
		struct DictEntry *elem = set->table[cursor];
		set->table[cursor] = NULL;
		size_t index = get_index(set, elem->key);
		set->table[index] = elem;
		cursor++;
	}
}

static struct DictEntry* get_entry(HashMap *set, void *key)
{
	size_t index = set->kops.hash(key, set->kops.arg) % set->capacity;
	while (set->table[index]) {
		if (set->kops.equals(key, set->table[index]->key, set->kops.arg))
			return set->table[index];
		index++;
	}
	return NULL;
}

int hashmap_insert(HashMap *set, void *key, void *value)
{
	if (set->size > 2*(set->capacity / 3)) {
		if (hashmap_extend(set))
			return 1;
	}

	struct DictEntry *e = get_entry(set, key);
	if (e) {
		set->kops.destroy(e->value, set->kops.arg);
		e->value = set->kops.copy(value, set->kops.arg);
	} else {
		struct DictEntry *new = malloc(sizeof(struct DictEntry));
		if (!new)
			return 1;
		new->hash = set->kops.hash(key, set->kops.arg);
		new->key = set->kops.copy(key, set->kops.arg);
		new->value = set->vops.copy(value, set->vops.arg);
		size_t index = get_index(set, key);
		set->table[index] = new;
		set->size++;
	}
	return 0;
}

void* hashmap_get(HashMap *set, void *key)
{
	size_t index = set->kops.hash(key, set->kops.arg) % set->capacity;
	while (set->table[index]) {
		if (set->kops.equals(key, set->table[index]->key, set->kops.arg))
			return set->table[index]->value;
		index++;
	}
	return NULL;
}

int hashmap_remove(HashMap *set, void *key)
{
	size_t index = set->kops.hash(key, set->kops.arg) % set->capacity;
	while (set->table[index]) {
		if (set->kops.equals(key, set->table[index]->key, set->kops.arg)) {
			set->kops.destroy(set->table[index]->key, set->kops.arg);
			set->vops.destroy(set->table[index]->value, set->vops.arg);
			free(set->table[index]);
			set->table[index] = NULL;
			rectify(set, index);
			return 0;
		}
		index++;
	}
	return 0;
}

int inteq(const void *d1, const void *d2, void *arg)
{
	return ((int) d1) == ((int) d2);
}

unsigned long inthash (const void *data, void *arg)
{
	int num = (int) data;
	return (unsigned long) num * 2654435761;
}

void intfree(void *data, void *arg)
{
}

void* intcopy(const void *data, void *arg) {return (void *) data;}

struct KeyOps intkey(void) {
	struct KeyOps intops = {inteq,inthash,intfree,intcopy,0};
	return intops;
}

struct ValOps intval(void) {
	struct ValOps intops = {intfree,intcopy,0};
	return intops;
}
