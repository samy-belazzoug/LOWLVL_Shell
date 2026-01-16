# Configuration variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iheader
TARGET = src/nanoshell
SRC = nanoshell.c imp/ns_imp.c
OBJ = $(SRC:.c=.o)

# Default rule
all: $(TARGET)

# Compilation of the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Compilation successful: $(TARGET)"

# Object file compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Object file cleaning
clean:
	rm -f $(OBJ)
	@echo "Object files removed"

# Complete cleaning
fclean: clean
	rm -f $(TARGET)
	@echo "Complete cleaning done."

# Complete recompilation
re: fclean all

.PHONY: all clean fclean re