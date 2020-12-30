APP_EXEC ?= app
TEST_EXEC ?= test
CC = gcc
CC_FLAGS = -c -Wall -Wextra -g3

# Include files. #
INC := -Iinclude

# Project directories. #
SRC_DIR ?= src
BUILD_DIR ?= build
BIN_DIR ?= bin
MODULE_DIR ?= modules
UNIT_TESTING_DIR ?= tests

# Building fundamentals for the main app. #
APP_SOURCES := $(shell find $(SRC_DIR) -type f -name *.c)
APP_OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(APP_SOURCES:.c=.o))

MODULES += $(shell find $(MODULE_DIR) -type f -name *.c)
MODULES_OBJECTS += $(patsubst $(MODULE_DIR)/%, $(BUILD_DIR)/%, $(MODULES:.c=.o))


# Building fundamentals for the unit tests. #
TEST_SOURCES := $(shell find $(UNIT_TESTING_DIR) -type f -name *.c)
TEST_OBJECTS := $(patsubst $(UNIT_TESTING_DIR)/%, $(BUILD_DIR)/%, $(TEST_SOURCES:.c=.o))

TEST_SOURCES += $(SRC_DIR)/clique.c $(SRC_DIR)/util.c $(SRC_DIR)/dataset_parsing.c
TEST_OBJECTS += $(BUILD_DIR)/util.o $(BUILD_DIR)/clique.o $(BUILD_DIR)/dataset_parsing.o # Add some extra dependancies

# Main app #
$(APP_EXEC): $(APP_OBJECTS) $(MODULES_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$(APP_EXEC) -lm

# Unit testing #
$(TEST_EXEC): $(TEST_OBJECTS) $(MODULES_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$(TEST_EXEC)


# Dependancies building #
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(MODULES_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $(INC) -o $@ $<

$(BUILD_DIR)/%.o: $(UNIT_TESTING_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $(INC) -o $@ $<

$(BUILD_DIR)/%.o: $(MODULE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $(INC) -o $@ $<


.PHONY:
	clean

clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*
