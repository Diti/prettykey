CFLAGS	?= -Werror
LDFLAGS ?=
LDLIBS	?= -lutil
ALL_CFLAGS=$(CFLAGS) -Wall -Wextra -std=c99 -pedantic

NAME=prettykey
OBJS=main.o

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

clean:
	$(RM) main.o

distclean: clean
	$(RM) $(NAME)

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c -o $@ $<
