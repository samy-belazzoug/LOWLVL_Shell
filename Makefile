# LOWLVL Shell - Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = 

# Directories
SRC_DIR = src
OBJ_DIR = .
BIN = bashr

# Source files
SOURCES = $(SRC_DIR)/bashr.c \
          $(SRC_DIR)/executor.c \
          $(SRC_DIR)/builtins.c \
          $(SRC_DIR)/utils.c

# Object files
OBJECTS = $(OBJ_DIR)/bashr.o \
          $(OBJ_DIR)/executor.o \
          $(OBJ_DIR)/builtins.o \
          $(OBJ_DIR)/utils.o

# Main target
all: $(BIN)

# Link object files to create executable
$(BIN): $(OBJECTS)
	@echo "Linking $(BIN)..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built $(BIN)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/bashr.h
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(BIN)
	@echo "Clean complete"

# Full rebuild
rebuild: clean all

# Help
help:
	@echo "LOWLVL Shell Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the shell (default)"
	@echo "  make all      - Build the shell"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC=$(CC)"
	@echo "  CFLAGS=$(CFLAGS)"

# Phony targets (not files)
.PHONY: all clean rebuild help
