# MatCom Invasion — Makefile
# Native (gcc), WASM (emcc) and tests.

CC      ?= gcc
EMCC    ?= emcc

SRC_DIR  := src
BUILD    := build
WASM_OUT := web/public

CFLAGS   := -std=c11 -O2 -Wall -Wextra -Werror -Isrc
WASM_FLAGS := -std=c11 -O2 -Wall -Wextra -Werror -Isrc

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(SRCS))

.PHONY: all native wasm test coverage coverage-gate clean

all: native

native: invasion_cli

invasion_cli: $(OBJS) $(BUILD)/main.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

wasm: $(WASM_OUT)/matcom_logic.js $(WASM_OUT)/matcom_logic.wasm

$(WASM_OUT)/matcom_logic.js: $(SRCS) | $(WASM_OUT)
	$(EMCC) $(WASM_FLAGS) \
		$(filter-out $(SRC_DIR)/main.c,$(SRCS)) \
		-o $(WASM_OUT)/matcom_logic.js \
		-s MODULARIZE=1 \
		-s EXPORT_NAME='createMatComModule' \
		-s EXPORTED_FUNCTIONS='["_game_create","_game_destroy","_game_set_mode","_game_start","_game_update","_game_set_ship","_game_fire","_game_pause","_game_resume","_game_get_state","_malloc","_free"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","HEAPU8"]' \
		-s ENVIRONMENT='web'

test:
	$(MAKE) -C tests test

coverage:
	$(MAKE) -C tests coverage

coverage-gate:
	$(MAKE) -C tests coverage-gate

$(BUILD) $(WASM_OUT):
	mkdir -p $@

clean:
	rm -rf $(BUILD) invasion_cli $(WASM_OUT)/matcom_logic.js $(WASM_OUT)/matcom_logic.wasm
