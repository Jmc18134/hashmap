#include <stdio.h>
#include "hashmap.h"

int main(void) {
	HashMap *hm = new_hashmap(intkey(), intval());
	for (int i=0, j=0; i<100; i++, j--) {
		hashmap_insert(hm, (void *)i, (void*)j);
	}
	for (int i=25; i<75; i++) {
		hashmap_remove(hm, (void*)i);
	}
	for (int i=0; i<100; i++) {
		int res = (int) hashmap_get(hm, i);
		printf("%d:%d\n", res ? res : -69);
	}
	destroy_hashmap(hm);
	return 0;
}
