# LOWLVL Shell - Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
BIN = bashr
SRC = bashr.c

all: $(BIN)

$(BIN): $(SRC)
	@echo "Building $(BIN)..."
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)
	@echo "✓ Built $(BIN)"

clean:
	@echo "Cleaning..."
	rm -f $(BIN) *.o
	@echo "✓ Clean"

rebuild: clean all

help:
	@echo "LOWLVL Shell Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make       - Build shell"
	@echo "  make clean - Remove artifacts"
	@echo "  make help  - Show this"

.PHONY: all clean rebuild help
