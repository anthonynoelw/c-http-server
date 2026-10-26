CC      = clang
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Iinclude -g
LDFLAGS =

BUILD_DIR = build/Debug
TARGET    = $(BUILD_DIR)/outDebug

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.c $(wildcard include/*.h) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build
