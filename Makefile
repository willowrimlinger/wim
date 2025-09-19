CC=gcc
CFLAGS=-I -Wall -Wextra -pedantic -g

LIBS=-lncurses

DEPS = $(wildcard *.h)

OBJ_DIR = obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(wildcard *.c))


$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(DEPS) $(OBJ_DIR)
		$(CC) -c -o $@ $< $(CFLAGS)

mim: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
		rm -rf $(OBJ_DIR)
		rm -f mim
