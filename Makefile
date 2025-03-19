CC = gcc
CFLAGS = -Wall -Werror

test: gb.o verifier.o
	$(CC) -o $@ $^
