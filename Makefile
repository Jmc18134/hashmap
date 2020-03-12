CC=gcc -fPIC
CFLAGS=-Wall -Wextra -std=c18 -pedantic -lm
TESTFLAGS=-g
LIBFLAGS=-O2
LDFLAGS=-shared
LIBNAME=libhashmap.so
DEPS=hashmap.h
TESTFILES=test.o
OBJ=hashmap.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(TESTFLAGS)

lib: $(OBJ)
	$(CC) $^ $(CFLAGS) $(LIBFLAGS) $(LDFLAGS) -o $(LIBNAME)

test: $(TESTFILES) $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(TESTFLAGS)

.PHONY: clean
clean:
	rm -f *.o

veryclean:
	rm -f *.o $(LIBNAME) test
