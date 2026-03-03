CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror

all:
	$(CC) $(CFLAGS) -o output src/main.c src/cpu/cpu.c