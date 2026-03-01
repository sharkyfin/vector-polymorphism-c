CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
LDLIBS = -lm

BUILD_DIR = build
APP_BIN = $(BUILD_DIR)/vector_lab
TEST_BIN = $(BUILD_DIR)/run_tests

all: $(APP_BIN)

$(APP_BIN): main.c utils.c vector.c field_info.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) main.c utils.c vector.c field_info.c -o $(APP_BIN) $(LDLIBS)

$(TEST_BIN): test_vector.c vector.c field_info.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) test_vector.c vector.c field_info.c -o $(TEST_BIN) $(LDLIBS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
