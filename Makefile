
CC=gcc

BINS=modexp

all: $(BINS)

modexp: modexp.c
	$(CC) -Wall -g -o modexp modexp.c -lm
clean:
	rm $(BINS)


