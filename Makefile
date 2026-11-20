CC ?= cc
CFLAGS ?= -std=c11 -O3 -Wall -Wextra -pedantic
LDLIBS ?= -lm
.PHONY: all check clean
all: generate verify
verify: verify.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)
generate: generate.c
	$(CC) $(CFLAGS) $< -o $@
check: verify
	./verify edges.txt
clean:
	rm -f verify generate clean_state.csv results.csv

capacity: associative.c
	$(CC) $(CFLAGS) associative.c -lm -o associative
	./associative
