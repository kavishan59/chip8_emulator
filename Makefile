
CC = gcc
CFLAGS = -MMD -MP -I/usr/include/SDL2
LDFLAGS = -lSDL2 -lSDL2_mixer

SRC = main.c chip8.c graphic.c opcodes.c

OBJ_DIR = .obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))
DEP = $(OBJ:.o=.d)

TARGET = chip8

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

-include $(DEP)

