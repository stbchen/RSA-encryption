CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: encrypt decrypt keygen

encrypt: encrypt.o rsa.o numtheory.o randstate.o
	$(CC) -o encrypt encrypt.o rsa.o numtheory.o randstate.o $(LFLAGS)

decrypt: decrypt.o rsa.o numtheory.o randstate.o
	$(CC) -o decrypt decrypt.o rsa.o numtheory.o randstate.o $(LFLAGS)

keygen: keygen.o randstate.o numtheory.o rsa.o
	$(CC) -o keygen keygen.o randstate.o numtheory.o rsa.o $(LFLAGS)

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) -c encrypt.c

decrypt.o: decrypt.c
	$(CC) $(CFLAGS) -c decrypt.c

keygen.o: keygen.c
	$(CC) $(CFLAGS) -c keygen.c

numtheory.o: numtheory.c
	$(CC) $(CFLAGS) -c numtheory.c

randstate.o: randstate.c
	$(CC) ${CFLAGS} -c randstate.c

rsa.o: rsa.c
	$(CC) $(CFLAGS) -c rsa.c

debug: CFLAGS += -g

debug: all

clean:
	rm -f *.o
	rm -f encrypt decrypt keygen

format:
	clang-format -i -style=file *.[ch]
