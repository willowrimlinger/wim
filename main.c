/**
 * @file main.c
 * @author Willow Rimlinger
 *
 * wim - a stupid text editor
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include "log.h"
#include "types.h"
#include "fileproxy.h"
#include "motions.h"
#include "insert.h"

static const int NORMAL_KEYS_LEN = 96;
static const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>? ";

void print_num(int y, int x, int num) { //FIXME remove
        move(y, x);
        clrtoeol();
        char str[3];
        sprintf(str, "%d", num);
        printw(str);
}

static void loop(FileProxy fp, const char *filename) {
    View view = {0, 0, 0, 0, 0};
    while (1) {
        print_fp(fp, view);
        move_cur(view);
        refresh();
        int key = getch();
        switch (key) {
            case KEY_UP:
                view = move_up(fp, view);
                break;
            case KEY_DOWN:
                view = move_down(fp, view);
                break;
            case KEY_LEFT:
                view = move_left(fp, view);
                break;
            case KEY_RIGHT:
                view = move_right(fp, view);
                break;
            case KEY_END:
                view = move_to_eol(fp, view);
                break;
            case KEY_HOME:
                view = move_to_bol(fp, view);
                break;
            case KEY_ENTER:
            case '\n':
            case '\r':
                view = insert_newline(&fp, view);
                break;
            case KEY_BACKSPACE:
                view = backspace(&fp, view);
                break;
            case KEY_NPAGE: // FIXME
                write_fp(fp, filename);
                break;
        }
        // text insertion
        for (int i = 0; i < NORMAL_KEYS_LEN; i++) {
            if (key == NORMAL_KEYS[i]) {
                view = insert_char(key, &fp, view);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    clear_log();

    if (argc != 2) {
        printf("Usage: wim <filename>\n");
        return EXIT_FAILURE;
    }

    // open file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "File \"%s\" not found.\n", argv[1]);
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
    loop(fp, argv[1]);

    free_fp(fp);
    endwin();
    return EXIT_SUCCESS;
}
