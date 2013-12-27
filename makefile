CC=gcc
LIBS=-lncurses
DEPS=*.h

main: $(DEPS) *.c
	$(CC) *.c -o main $(LIBS) -g
