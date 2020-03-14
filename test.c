#include <stdio.h>
#include "hashmap.h"

int main(void) {
	HashMap *hm = new_hashmap();

	for (int i=0, j=100; i<100; i++, j--) {
		if (hashmap_insert(hm, i, j)) {
			printf("Error on inserting %d:%d\n", i, j);
		}
	}

	int res = 0;
	for (int i=25; i<75; i++) {
		hashmap_remove(hm, i);
	}

	for (int i=0; i<100; i++) {
		int err = hashmap_get(hm, i, &res);
		printf("%d:%d\n", i, err ? -69 : res);
	}

	destroy_hashmap(hm);
	return 0;
}
