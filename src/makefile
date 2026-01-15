# Variables de configuration
CC = gcc
CFLAGS = -Wall -Wextra -Iheader
TARGET = exe/bashr
SRC = bashr.c imp/my_parsing.c
OBJ = $(SRC:.c=.o)

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJ)
	@mkdir -p exe
	$(CC) $(CFLAGS) -o $@ $^
	@echo "✓ Compilation réussie : $(TARGET)"

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets
clean:
	rm -f $(OBJ)
	@echo "✓ Fichiers objets supprimés"

# Nettoyage complet
fclean: clean
	rm -f $(TARGET)
	@echo "✓ Nettoyage complet"

# Recompilation complète
re: fclean all

.PHONY: all clean fclean re