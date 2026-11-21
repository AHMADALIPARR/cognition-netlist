CC ?= cc
CFLAGS ?= -std=c11 -O3 -Wall -Wextra -pedantic
LDLIBS ?= -lm
.PHONY: all check clean
all: generate verify
bin:
	mkdir -p bin
verify: src/verify.c | bin
	$(CC) $(CFLAGS) $< -o bin/verify $(LDLIBS)
generate: src/generate.c | bin
	$(CC) $(CFLAGS) $< -o bin/generate
check: verify
	./bin/verify data/edges.txt
clean:
	rm -rf bin data/clean_state.csv data/results.csv

capacity: src/associative.c | bin
	$(CC) $(CFLAGS) src/associative.c -lm -o bin/associative
	./bin/associative
