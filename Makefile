CC ?= gcc

# Keep warnings enabled and include debug symbols so objdump/gdb are useful.
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -g

# Let main.c include headers from include/.
CPPFLAGS ?= -Iinclude

# Build artifacts stay under build/ so source files remain clean.
BUILD_DIR := build
TARGET := $(BUILD_DIR)/bitlab

# The final executable is linked from one C file and one Assembly file.
SOURCES := src/main.c src/bitops.S

.PHONY: all run clean

# Default target: compile the executable.
all: $(TARGET)

# Rebuild when sources or the public header change.
$(TARGET): $(SOURCES) include/bitops.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

# Create the build directory only when needed.
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Example execution used for a quick smoke test.
run: $(TARGET)
	./$(TARGET) "hello blockchain"

# Remove generated files.
clean:
	rm -rf $(BUILD_DIR)
