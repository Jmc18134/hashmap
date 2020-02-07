#ifndef _HASHMAP_H_
#define _HASHMAP_H_

struct KeyOps {
	int (*equals)(const void *d1, const void *d2, void *arg);
	unsigned long (*hash)(const void *data, void *arg);
	void (*destroy)(void *data, void *arg);
	void *(*copy)(const void *data, void *arg);
	void *arg;
};

struct ValOps {
	void (*destroy)(void *data, void *arg);
	void *(*copy)(const void *data, void *arg);
	void *arg;
};

typedef struct HashMap HashMap;

int hashmap_insert(HashMap *set, void *key, void *value);
HashMap* new_hashmap(struct KeyOps kops, struct ValOps vops);
void destroy_hashmap(HashMap *set);
int hashmap_insert(HashMap *set, void *key, void *value);
void* hashmap_get(HashMap *set, void *key);
int hashmap_remove(HashMap *set, void *key);
struct KeyOps intkey(void);
struct ValOps intval(void);
#endif
