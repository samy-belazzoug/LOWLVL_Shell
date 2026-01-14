CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS = 

SRCS = src/bashr.c src/executor.c src/builtins.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bashr

.PHONY: all clean test help

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "✓ Built $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "✓ Cleaned build artifacts"

test: $(TARGET)
	@echo "Running basic tests..."
	@echo "cd /tmp" | ./$(TARGET)
	@echo "pwd" | ./$(TARGET)
	@echo "echo Hello World" | ./$(TARGET)
	@echo "✓ Basic tests passed"

help:
	@echo "LOWLVL Shell - Build targets:"
	@echo "  make all    - Build the shell"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make test   - Run basic tests"
	@echo "  make help   - Show this help message"
