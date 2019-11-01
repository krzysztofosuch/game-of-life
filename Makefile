CC = gcc

game: main.c
	$(CC) main.c -Wall -lncurses -o main -ggdb -O0

