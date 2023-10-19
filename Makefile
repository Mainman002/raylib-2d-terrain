CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -I/opt/homebrew/Cellar/raylib/4.5.0/include -I/opt/homebrew/Cellar/chipmunk/7.0.3/include
LDFLAGS=-L/opt/homebrew/Cellar/raylib/4.5.0/lib -L/opt/homebrew/Cellar/chipmunk/7.0.3/lib
LDLIBS=-lraylib -lchipmunk -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

SRC_DIR=src
BUILD_DIR=build

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

EXECUTABLE=$(BUILD_DIR)/app_name

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
