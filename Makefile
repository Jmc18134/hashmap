CC=gcc
CFLAGS=-Wall -Wextra -std=c18 -pedantic -g -lm
DEPS=hashmap.h
OBJ=main.o hashmap.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dict_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o

cleanbuild:
	clean shell
