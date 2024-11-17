# Makefile for building the tests

# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SRC_FILES = $(SRC_DIR)/tests.c $(SRC_DIR)/main.c
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Output executable
OUTPUT = $(SRC_DIR)/tests

# Targets and rules
all: $(OUTPUT)

$(OUTPUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(OUTPUT)

.PHONY: all clean
