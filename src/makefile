# Variables de configuration
CC = gcc
CFLAGS = -Wall -Wextra -Iheader
TARGET = exe/nanoshell
SRC = nanoshell.c imp/ns_imp.c
OBJ = $(SRC:.c=.o)

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJ)
	@mkdir -p exe
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Compilation sucess : $(TARGET)"

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Object file cleaning
clean:
	rm -f $(OBJ)
	@echo "Object files cleaned"

# Nettoyage complet
fclean: clean
	rm -f $(TARGET)
	@echo "Complete cleaned done."

# Recompilation complète
re: fclean all

.PHONY: all clean fclean re