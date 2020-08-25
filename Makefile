# Build program
all: server client

CC = gcc
SRCS1 = server.c
CFLAGS1 = -o server -Wall -Werror
server: server.c
	$(CC) $(SRCS1) $(CFLAGS1)

SRCS2 = client.c
CFLAGS2 = -o client -Wall -Werror
client: client.c
	$(CC) $(SRCS2) $(CFLAGS2)

# Clean regenerable file
.PHONY: clean
clean:
	rm -f server client

# Regression tests
PROGS = server client
TESTS = test1.sh test2.sh test3.sh test4.sh test5.sh
tests: $(PROGS) $(TESTS)
	sh test1.sh
	sh test2.sh
	sh test3.sh
	sh test4.sh
	sh test5.sh