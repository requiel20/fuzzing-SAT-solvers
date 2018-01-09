CC=gcc
CFLAGS=-I. -Wall -Wextra -Wpedantic $(flags)
COVFLAGS=-g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls -ftest-coverage -fprofile-arcs
SANFLAGS=-fsanitize=address -fsanitize=undefined -fsanitize-recover=all

DEPS = $(wildcard *.h)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

sat: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: ub
ub: CFLAGS += $(COVFLAGS) $(SANFLAGS)
ub: clean sat;

.PHONY: func
func: CFLAGS += $(COVFLAGS)
func: clean sat;

.PHONY: clean
clean:
	rm -f *.o *.gcov *.gcda *.gcno sat
