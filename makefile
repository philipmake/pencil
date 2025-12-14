# === Compiler and flags ===
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude

# === Directories ===
SRCDIR = src
OBJDIR = build
BINDIR = .
BIN = $(BINDIR)/pencil

# === Source and object files ===
SRC := $(shell find $(SRCDIR) -type f -name '*.c' 2>nul)
SRC += main.c

# Replace src/... with build/... and .c with .o
OBJ := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJ := $(OBJ:main.c=$(OBJDIR)/main.o)

# === Default target ===
all: $(OBJDIR) $(BIN)

# Create build directory
$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@for dir in $(shell find $(SRCDIR) -type d 2>nul); do \
		mkdir -p $(OBJDIR)/$${dir#$(SRCDIR)/}; \
	done

# Compile each .c file into .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile main.c separately
$(OBJDIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link all .o files
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# === Utility targets ===
clean:
	rm -rf $(OBJDIR) $(BIN)
	@echo "Cleaned build files."

run: $(BIN)
	./$(BIN)

.PHONY: all clean run
