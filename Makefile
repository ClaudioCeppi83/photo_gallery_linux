# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)

# Source and header directories
SRC_DIR = src
INCLUDE_DIR = src

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
TARGET = photo_gallery

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $@ $^ $(GTK_LIBS)

# Compile source files to object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

# Clean up object files
clean:
	rm -f $(OBJ)

# Clean up object files and the executable
fclean: clean
	rm -f $(TARGET)

# Clean up everything and recompile
re: fclean all
