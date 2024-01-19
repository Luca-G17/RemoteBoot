CC=gcc
CFLAGS=-Wall -std=c11
SRC_DIR=src
BUILD_DIR=build
EXECUTABLE=$(BUILD_DIR)/main
OBJECT_FILE=$(BUILD_DIR)/main.o
SOURCE_FILE=$(SRC_DIR)/main.c

default: build

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build: $(OBJECT_FILE)
	$(CC) -o $(EXECUTABLE) $(OBJECT_FILE)

clean:
	rm $(BUILD_DIR)/*
