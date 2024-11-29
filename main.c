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

const size_t BUF_INCR = 64;

const int NORMAL_KEYS_LEN = 96;
const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>?";

typedef struct CurPos {
    int y;
    int x;
    int desired_x;
};

CurPos move_up(char *buffer, size_t *buf_idx, CurPos pos) {
    // loop back through the buffer until you hit a new line
    size_t start_idx = *buf_idx;
    size_t delta = 0;
    while (buffer[start_idx-delta] != '\n') {
        if (start_idx-delta == 0) {
            return pos; // can't move up, first line
        }
        delta++;
    }
    // keep going counting the entire length of the above line
    size_t above_line_length = 0;
    while (buffer[start_idx-delta] != '\n' && start_idx-delta >= 0) {
        above_line_length++;
        delta++;
    }
    // if the above line length is >= your desired x
    if (above_line_length >= desired_x) {
        // move that many spaces from the beginning of the above line
        // TODO word wrap and text extending past right edge
        *buf_idx -= delta;
        // TODO make new curPos
    }
    // if not, move to the end of the above line
    // either way, update the desired_x
}

// TODO in the move left and right functions, you will have to update the desired_x to be the new current x

void loop(char *buffer) {
    int cur_y = 0;
    int cur_x = 0;
    size_t buf_idx = 0;
    printw(buffer);
    refresh();
    while (1) {
        move(cur_y, cur_x);
        int key = getch();
        switch (key) {
            case KEY_UP:
                CurPos pos = move_up(buffer, &buf_idx);
                move(pos.y, pos.x);
                break;
//            case KEY_DOWN:
//                CurPos pos = move_down(&buf_idx);
//                move(pos.y, pos.x);
//                break;
//            case KEY_LEFT:
//                CurPos pos = move_left(&buf_idx);
//                move(pos.y, pos.x);
//                break;
//            case KEY_RIGHT:
//                CurPos pos = move_right(&buf_idx);
//                move(pos.y, pos.x);
//                break;
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
        printf("Usage: wim <filename>\n");
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

    // main program loop
    loop(buffer);

    free(buffer);
    endwin();
    return 0;
}
