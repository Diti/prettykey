NAME=prettykey

CFLAGS?=-Werror

ALL_CFLAGS=$(CFLAGS) -Wall -Wextra -std=c99 -pedantic

all: $(NAME)

$(NAME): main.o
	$(CC) $(ALL_CFLAGS) -o $@ $^ -lutil

clean:
	$(RM) main.o

distclean: clean
	$(RM) $(NAME)
