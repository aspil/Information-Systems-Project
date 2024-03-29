APP_EXEC ?= app
TEST_EXEC ?= test
PROFILING_FLAGS =
LIB = -pthread -lm
override CFLAGS = -c -Wall -Wextra -g3 ${VERBOSE}
CC = gcc $(PROFILING_FLAGS) -pthread $(PROFILING_FLAGS) 

# Include files. #
INC := -Iinclude

# Project directories. #
APP_DIR ?= app
SRC_DIR ?= src
BUILD_DIR ?= build
BIN_DIR ?= bin
MODULE_DIR ?= modules
UNIT_TESTING_DIR ?= tests

# Building fundamentals for the main app. #
APP_SOURCES := $(shell find $(SRC_DIR)/ -type f -name *.c -printf "%f\n")
APP_SOURCES += $(shell find $(APP_DIR)/ -type f -name *.c -printf "%f\n")

APP_OBJECTS := $(patsubst %, $(BUILD_DIR)/%, $(APP_SOURCES:.c=.o))

MODULES += $(shell find $(MODULE_DIR) -type f -name *.c -printf "%f\n")
MODULES_OBJECTS += $(patsubst %, $(BUILD_DIR)/%, $(MODULES:.c=.o))

# Building fundamentals for the unit tests. #
TEST_SOURCES := $(shell find $(UNIT_TESTING_DIR) -type f -name *.c -printf "%f\n")
TEST_OBJECTS := $(patsubst %, $(BUILD_DIR)/%, $(TEST_SOURCES:.c=.o))

TEST_SOURCES += $(SRC_DIR)/clique/clique.c $(SRC_DIR)/util/argparse.c $(SRC_DIR)/util/util.c $(SRC_DIR)/util/text_preprocessing.c $(SRC_DIR)/util/dataset_parsing.c 
TEST_OBJECTS += $(BUILD_DIR)/util.o $(BUILD_DIR)/clique.o $(BUILD_DIR)/argparse.o $(BUILD_DIR)/dataset_parsing.o $(BUILD_DIR)/text_preprocessing.o# Add some extra dependancies

# Main app #
$(APP_EXEC): $(APP_OBJECTS) $(MODULES_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LIB) $^ -o $(BIN_DIR)/$(APP_EXEC) $(LIB)

$(BUILD_DIR)/main.o: $(APP_DIR)/main.c
	$(CC) $(CFLAGS) $(INC) -o $@ $<

# Unit testing #
$(TEST_EXEC): $(TEST_OBJECTS) $(MODULES_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$(TEST_EXEC)

# Dependancies building #
$(BUILD_DIR)/%.o: $(SRC_DIR)/*/%.c $(MODULES_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) -o $@ $<

$(BUILD_DIR)/%.o: $(UNIT_TESTING_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) -o $@ $<

$(BUILD_DIR)/%.o: $(MODULE_DIR)/*/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) -o $@ $<


.PHONY:
	clean

clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*
