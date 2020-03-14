#ifndef _HASHMAP_H_
#define _HASHMAP_H_

typedef struct HashMap HashMap;

HashMap *new_hashmap(void);
void destroy_hashmap(HashMap *map);

int hashmap_insert(HashMap *map, int key, int value);
int hashmap_remove(HashMap *map, int key);

int hashmap_get(HashMap *map, int key, int *result);
#endif
