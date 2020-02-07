#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
int main(void)
{
	struct KeyOps kop = intkey();
	struct ValOps vop = intval();
	HashMap *new = new_hashmap(kop, vop);
	for (int i=0, j=50; i<50; i++, j--)
		hashmap_insert(new, (void*) i, (void*) j);
	for (int i=20, j=40; i<40; i++, j--)
		hashmap_remove(new, (void*) i);
	for (int i=0; i<50; i++) {
		void *out = hashmap_get(new, (void*) i);
		printf("%d:%d\n", i, out == NULL ? 42: out);
	}
	destroy_hashmap(new);
	return 0;
}
