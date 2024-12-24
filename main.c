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

#include "log.h"
#include "types.h"
#include "fileproxy.h"
#include "motions.h"
#include "insert.h"
#include "display.h"

static const int NORMAL_KEYS_LEN = 96;
static const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>? ";

static void loop(FileProxy fp, const char *filename) {
    View view = {0, 0, LINES - 1, COLS, 0, 0, 0};
    char mode = 'n'; // i - insert, n - normal
    while (1) {
        display(mode, fp, view);
        int key = getch();
        if (mode == 'i') { // insert mode
            switch (key) {
                case KEY_UP:
                    move_up(fp, &view, mode);
                    break;
                case KEY_DOWN:
                    move_down(fp, &view, mode);
                    break;
                case KEY_LEFT:
                    move_left(fp, &view);
                    break;
                case KEY_RIGHT:
                    move_right(fp, &view, mode);
                    break;
                case KEY_END:
                    move_to_eol(fp, &view, mode);
                    break;
                case KEY_HOME:
                    move_to_bol(fp, &view);
                    break;
                case KEY_ENTER:
                case '\n':
                case '\r':
                    insert_newline(&fp, &view);
                    break;
                case KEY_BACKSPACE:
                    backspace(&fp, &view, mode);
                    break;
                case KEY_DC:
                    delete(&fp, &view, mode);
                    break;
                case 27:
                    mode = 'n';
                    break;
            }
            // text insertion
            for (int i = 0; i < NORMAL_KEYS_LEN; i++) {
                if (key == NORMAL_KEYS[i]) {
                    insert_char(key, &fp, &view, mode);
                }
            }
        } else { // normal mode
            switch (key) {
                case KEY_UP:
                case 'k':
                    move_up(fp, &view, mode);
                    break;
                case KEY_DOWN:
                case 'j':
                    move_down(fp, &view, mode);
                    break;
                case KEY_LEFT:
                case 'h':
                    move_left(fp, &view);
                    break;
                case KEY_RIGHT:
                case 'l':
                    move_right(fp, &view, mode);
                    break;
                case KEY_END:
                case '$':
                    move_to_eol(fp, &view, mode);
                    break;
                case KEY_HOME:
                case '0':
                    move_to_bol(fp, &view);
                    break;
                case KEY_ENTER:
                case '\n':
                case '\r':
                    // TODO move down, move to first non whitespace char
                    break;
                case KEY_BACKSPACE:
                    // TODO move left, or move up a line if on first char
                    break;
                case KEY_DC:
                case 'x':
                    delete(&fp, &view, mode);
                    break;
                case 'i':
                    mode = 'i';
                    break;
                case 'A':
                    mode = 'i';
                    move_to_eol(fp, &view, mode);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    clear_log();

    if (argc != 2) {
        printf("Usage: mim <filename>\n");
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
