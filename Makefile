PKG_CONFIG ?= $(shell command -v pkgconf 2>/dev/null || command -v pkg-config 2>/dev/null || echo pkg-config)
CC ?= cc
PREFIX ?= /usr/local

TARGET := build/garmin-mtp
ALIAS_TARGET := build/garminmtp
TEST_TARGET := build/test-garmin-mtp

SRC := \
	src/cli.c \
	src/commands.c \
	src/file_type.c \
	src/main.c \
	src/manual.c \
	src/memory.c \
	src/mtp_device.c \
	src/mtp_tree.c \
	src/path.c \
	src/process_conflict.c \
	src/progress.c

TEST_SRC := \
	tests/cli_test.c \
	tests/file_type_test.c \
	tests/manual_test.c \
	tests/mtp_tree_test.c \
	tests/path_test.c \
	tests/process_conflict_test.c \
	tests/test_main.c

TEST_SUPPORT_SRC := \
	src/cli.c \
	src/file_type.c \
	src/manual.c \
	src/memory.c \
	src/mtp_tree.c \
	src/path.c \
	src/process_conflict.c

CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Isrc $(shell $(PKG_CONFIG) --cflags libmtp)
LDLIBS += $(shell $(PKG_CONFIG) --libs libmtp)

.PHONY: all check clean install test

all: $(TARGET) $(ALIAS_TARGET)

$(TARGET): $(SRC) | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(SRC) $(LDFLAGS) $(LDLIBS)

$(ALIAS_TARGET): $(TARGET)
	cp "$(TARGET)" "$@"

$(TEST_TARGET): $(TEST_SRC) $(TEST_SUPPORT_SRC) | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(TEST_SRC) $(TEST_SUPPORT_SRC) $(LDFLAGS) $(LDLIBS)

build:
	mkdir -p $@

test: $(TEST_TARGET)
	$(TEST_TARGET)

check: $(TARGET) $(ALIAS_TARGET) test
	$(TARGET) --help >/dev/null
	$(ALIAS_TARGET) --help >/dev/null
	$(TARGET) manual >/dev/null
	$(TARGET) menu >/dev/null

install: $(TARGET)
	install -d "$(PREFIX)/bin"
	install -m 0755 "$(TARGET)" "$(PREFIX)/bin/garmin-mtp"
	install -m 0755 "$(TARGET)" "$(PREFIX)/bin/garminmtp"

clean:
	rm -rf build
