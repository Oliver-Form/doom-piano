CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lasound
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = build/only-c

all: build $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	@mkdir -p build

clean:
	rm -f build/*.o $(TARGET)

.PHONY: all clean
