# Makefile для тестов

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
TEST_SOURCES = test_kernel.c test_memory.c
TEST_TARGETS = $(TEST_SOURCES:.c=)

.PHONY: all clean test

all: $(TEST_TARGETS)

test_kernel: test_kernel.c
	$(CC) $(CFLAGS) -o $@ $<

test_memory: test_memory.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@echo "Running kernel tests..."
	@./test_kernel
	@echo ""
	@echo "Running memory tests..."
	@./test_memory

clean:
	rm -f $(TEST_TARGETS)
