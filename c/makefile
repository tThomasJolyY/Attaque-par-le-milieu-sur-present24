# Nom de l'exécutable final
TARGET = attaque_milieu

# Compilateur
CC = gcc

# Options du compilateur
CFLAGS = -Wall -Wextra -std=c99

# Liste des fichiers sources (.c)
SRCS = const.c bit_operations.c couple.c tri.c cadencement.c chiffrement.c dechiffrement.c attaque_millieu.c

# Liste des fichiers objets générés à partir des sources
OBJS = $(SRCS:.c=.o)

# Règle par défaut pour compiler le projet
all: $(TARGET)

# Règle pour générer l'exécutable à partir des fichiers objets
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Règle pour compiler chaque fichier .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers objets et l'exécutable
clean:
	rm -f $(OBJS) $(TARGET)

# Tout nettoyer, y compris les fichiers de sauvegarde (~)
cleanall: clean
	rm -f *~
