CC=gcc
CFLAGS=-I -Wall -Wextra -pedantic -g

LIBS=-lncurses

DEPS = $(wildcard *.h)

OBJ_DIR = obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(wildcard *.c))


$(OBJ_DIR)/%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

mim: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
		rm -f $(OBJ_DIR)/*.o
		rm -f mim
