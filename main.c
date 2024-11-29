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

const size_t BUF_INCR = 64;

const int NORMAL_KEYS_LEN = 96;
const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>?";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: mim <filename>\n");
        return 1;
    }

    // open file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("File \"%s\" not found.\n", argv[1]);
        return 1;
    }
    // get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    // go back to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // move file into buffer
    size_t byte = sizeof(unsigned char);
    size_t buf_size = file_size + BUF_INCR;
    char *buffer = malloc(buf_size);
    fread(buffer, byte, buf_size / byte, file);
    fclose(file);

    initscr();
    keypad(stdscr, TRUE);

    int cur_x = 0;
    int cur_y = 0;
    printw(buffer);
    refresh();
    while (1) {
        move(cur_x, cur_y);
        int key = getch();
        switch (key) {
            case KEY_RIGHT:
                cur_y++;
                break;
            case KEY_LEFT:
                cur_y--;
                break;
            case KEY_UP:
                cur_x--;
                break;
            case KEY_DOWN:
                cur_x++;
                break;
        }
//        for (int i = 0; i < NORMAL_KEYS_SIZE; i++) {
//            if (key == NORMAL_KEYS[i]) {
//
//            }
//        }
    }

    free(buffer);
    endwin();
    return 0;
}
