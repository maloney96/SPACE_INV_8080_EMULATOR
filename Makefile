# Compiler
CC = clang

# Compiler flags
CFLAGS = -g -fcolor-diagnostics -fansi-escape-codes

# Source files
SRCS = main.c memory/memory.c memory/mem_utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = emulator

# Default target
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
    rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean