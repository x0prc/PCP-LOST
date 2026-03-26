# Compiler
CC := gcc

# Common flags
CFLAGS := -O2 -Wall -Wextra -std=gnu11

# Debug flags
DEBUG_FLAGS := -O0 -g

# Source files
SRCS := pcp_test.c msg_spray.c

# Output binary
TARGET := pcp_test

# Default build
all: $(TARGET)

# Link all sources into single binary
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

# Debug build (adds symbols, disables optimizations)
debug:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(TARGET) $(SRCS)

# Clean build artifacts
clean:
	rm -f $(TARGET)
