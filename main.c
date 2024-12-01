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

#include "fileproxy.h"
#include "motions.h"

const int NORMAL_KEYS_LEN = 96;
const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>?";

void print_num(int y, int x, int num) { //FIXME remove
        move(y, x);
        char str[3];
        sprintf(str, "%d", num);
        printw(str);
}

void loop(FileProxy fp) {
    Pos pos = {0, 0, 0};
    print_fp(fp);
    refresh();
    while (1) {
        move_cur(pos);
        int key = getch();
        switch (key) {
            case KEY_UP:
                pos = move_up(fp, pos);
                break;
            case KEY_DOWN:
                pos = move_down(fp, pos);
                break;
            case KEY_LEFT:
                pos = move_left(pos);
                break;
            case KEY_RIGHT:
                pos = move_right(fp, pos);
                break;
        }
        // text insertion
//        for (int i = 0; i < NORMAL_KEYS_SIZE; i++) {
//            if (key == NORMAL_KEYS[i]) {
//
//            }
//        }
    }
}

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
    char *buffer = malloc(file_size);
    fread(buffer, byte, file_size / byte, file);
    fclose(file);

    // split buffer into array of Lines
    FileProxy fp = split_buffer(buffer, file_size / byte);
    free(buffer);
    print_fp(fp);

    initscr();
    keypad(stdscr, TRUE);

    // main program loop
    loop(fp);

    free_fp(fp);
    endwin();
    return 0;
}
