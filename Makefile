CC = cc
CFLAGS = -std=c11 -O3 -Wall -Wextra -pedantic
LDFLAGS = -lm

.PHONY: all clean

all: generate verify associative

generate: generate.c
	$(CC) $(CFLAGS) -o $@ $<

verify: verify.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

associative: associative.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f generate verify associative
