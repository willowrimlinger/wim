/**
 * @file main.c
 * @author Max Rimlinger
 *
 * mim - a stupid text editor
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include "types.h"
#include "fileproxy.h"
#include "motions.h"
#include "insert.h"

static const int NORMAL_KEYS_LEN = 96;
static const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>?";

void print_num(int y, int x, int num) { //FIXME remove
        move(y, x);
        clrtoeol();
        char str[3];
        sprintf(str, "%d", num);
        printw(str);
}

static void loop(FileProxy fp) {
    View view = {0, 0, 0, 0, 0};
    print_fp(fp, view);
    refresh();
    while (1) {
        // movement
        print_num(30, 30, view.left_ch);
        move_cur(view);
        int key = getch();
        switch (key) {
            case KEY_UP:
                view = move_up(fp, view);
                break;
            case KEY_DOWN:
                view = move_down(fp, view);
                break;
            case KEY_LEFT:
                view = move_left(view);
                break;
            case KEY_RIGHT:
                view = move_right(fp, view);
                break;
        }
        // text insertion
        for (int i = 0; i < NORMAL_KEYS_LEN; i++) {
            if (key == NORMAL_KEYS[i]) {
                view = insert_char(key, fp, view);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: mim <filename>\n");
        return EXIT_FAILURE;
    }

    // open file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("File \"%s\" not found.\n", argv[1]);
        return EXIT_FAILURE;
    }
    // get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    // go back to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // move file into buffer
    size_t byte = sizeof(unsigned char);
    char *buffer = malloc(file_size);
    fread(buffer, byte, file_size / byte, file);
    fclose(file);

    // split buffer into array of Lines
    FileProxy fp = split_buffer(buffer, file_size / byte);
    free(buffer);

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    // main program loop
    loop(fp);

    free_fp(fp);
    endwin();
    return EXIT_SUCCESS;
}
