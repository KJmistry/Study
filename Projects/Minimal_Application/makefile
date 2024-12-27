# Compiler
CC = gcc

# Directories
SRC_DIR = src/application
BUILD_DIR = src/build

# Source and module directories
MODULE_DIRS = $(wildcard $(SRC_DIR)/*)

# Gather all .c files from the main and module directories
MAIN_SRC = $(SRC_DIR)/main.c
MODULE_SRCS = $(wildcard $(foreach dir,$(MODULE_DIRS),$(dir)/*.c))

# Object files
MAIN_OBJ = $(BUILD_DIR)/main.o
MODULE_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(MODULE_SRCS))

# Executable name
TARGET = $(BUILD_DIR)/app.bin

# Flags
CFLAGS = -Wall -g
LDFLAGS = 

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(MAIN_OBJ) $(MODULE_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile the main source file
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory
$(BUILD_DIR)/:
	mkdir -p $(BUILD_DIR)

# Compile all module source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)  # Create the directory for the object file
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove build artifacts
clean:
	rm -rf $(BUILD_DIR)/* $(TARGET)

# Phony targets
.PHONY: all clean
