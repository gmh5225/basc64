CC = cc
CFLAGS = -Wall -Wextra -std=c99 -g -O3
OUT = bin/basc64
SRCS = $(wildcard src/*.c)

all:
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS)

clean:
	rm -r bin
