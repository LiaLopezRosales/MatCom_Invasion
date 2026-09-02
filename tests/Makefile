# MatCom Invasion — Test suite (Criterion)

CC      ?= gcc
SRC_DIR := ../src

CRITERION_CFLAGS := $(shell pkg-config --cflags criterion)
CRITERION_LIBS   := $(shell pkg-config --libs criterion)

# Test build: with ASan/UBSan (Phase 3)
TEST_CFLAGS := -std=c11 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
               -I$(SRC_DIR) $(CRITERION_CFLAGS)
TEST_LIBS   := -fsanitize=address,undefined -lm $(CRITERION_LIBS)

SRC_FILES := $(filter-out $(SRC_DIR)/main.c,$(wildcard $(SRC_DIR)/*.c))
TEST_FILES := $(wildcard test_*.c)
COV_DIR := cov_build

# Coverage build flags (separate objects, do not mix with test objects)
COV_CFLAGS := -std=c11 -g -O0 -Wall -Wextra -Werror -fprofile-arcs -ftest-coverage \
              -I$(SRC_DIR) $(CRITERION_CFLAGS)
COV_LIBS   := -fprofile-arcs -ftest-coverage -lm $(CRITERION_LIBS)
COV_OBJS   := $(patsubst $(SRC_DIR)/%.c,$(COV_DIR)/%.o,$(SRC_FILES)) \
              $(patsubst %.c,$(COV_DIR)/%.o,$(TEST_FILES))

.PHONY: test coverage clean

test: test_runner
	./test_runner --verbose
	@echo "== TESTS PASSED =="

test_runner: $(TEST_FILES) $(SRC_FILES)
	$(CC) $(TEST_CFLAGS) -o $@ $(TEST_FILES) $(SRC_FILES) $(TEST_LIBS)

coverage: cov_runner
	@cd $(COV_DIR) && ../cov_runner --quiet > /dev/null 2>&1 || true
	@echo "== COVERAGE (gcov) =="
	@cd $(COV_DIR) && gcov -o . $(SRC_FILES:../%=../../%) 2>/dev/null | grep -E "File|Lines executed" || true

cov_runner: $(COV_OBJS) | $(COV_DIR)
	$(CC) -o $@ $(COV_OBJS) $(COV_LIBS)

$(COV_DIR)/%.o: $(SRC_DIR)/%.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c $< -o $@

$(COV_DIR)/%.o: %.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c $< -o $@

$(COV_DIR):
	mkdir -p $@

clean:
	rm -f test_runner cov_runner
	rm -rf $(COV_DIR)
	rm -f $(SRC_DIR)/*.gcda $(SRC_DIR)/*.gcno $(SRC_DIR)/*.gcov
