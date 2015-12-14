CFLAGS=-Wall -Wextra -pedantic
ALL_CFLAGS=$(CFLAGS)

all: main.o
	$(CC) $(ALL_CFLAGS) $^
