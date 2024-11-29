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

typedef struct CurPos_s {
    int y;
    int x;
    int desired_x;
} CurPos;

void print_num(int y, int x, int num) {
        move(y, x);
        char str[3];
        sprintf(str, "%d", num);
        printw(str);
}

CurPos move_up(char *buffer, size_t *buf_idx, CurPos pos) {
    // loop back through the buffer until you hit a new line
    size_t i = *buf_idx;
    while (buffer[i] != '\n') {
        if (i == 0) {
            return pos; // can't move up, first line
        }
        i--;
    }
    i--; // move past the \n
    // keep going counting the entire length of the above line
    size_t above_line_length = 0;
    while (buffer[i] != '\n' && i >= 0) {
        above_line_length++;
        i--;
    }

    // if the above line length is >= your desired x
    if (above_line_length >= pos.desired_x) {
        // move that many spaces from the beginning of the above line
        // TODO word wrap and text extending past right edge
        *buf_idx = i + pos.desired_x;
        CurPos new_pos = {pos.y-1, pos.desired_x, pos.desired_x};
        return new_pos;
    } else { // if not, move to the end of the above line
        *buf_idx = i + above_line_length;
        CurPos new_pos = {pos.y-1, above_line_length, pos.desired_x};
        return new_pos;
    }
}

CurPos move_down(char *buffer, size_t buf_size, size_t *buf_idx, CurPos pos) {
    // loop forward through the buffer until you hit a new line
    size_t i = *buf_idx;
    while (buffer[i] != '\n') {
        if (i == buf_size-1) {
            return pos; // can't move up, last line
        }
        i++;
    }
    // keep going counting the entire length of the below line
    size_t below_line_length = 0;
    while (buffer[i] != '\n' && i < buf_size) {
        below_line_length++;
        i++;
    }
    i++; // move i to be past the \n

    // if the below line length is >= your desired x
    if (below_line_length >= pos.desired_x) {
        // move that many spaces from the beginning of the below line
        // TODO word wrap and text extending past right edge
        *buf_idx = i - (below_line_length - pos.desired_x);
        CurPos new_pos = {pos.y+1, pos.desired_x, pos.desired_x};
        return new_pos;
    } else { // if not, move to the end of the below line
        *buf_idx = i;
        CurPos new_pos = {pos.y+1, below_line_length, pos.desired_x};
        return new_pos;
    }
}

// TODO in the move left and right functions, you will have to update the desired_x to be the new current x
CurPos move_right(char *buffer, size_t buf_size, size_t *buf_idx, CurPos pos) {
    if (*buf_idx + 1 != buf_size && buffer[*buf_idx+1] != '\n') {
        *buf_idx += 1;
        CurPos new_pos = {pos.y, pos.x+1, pos.x+1};
        return new_pos;
    }
    return pos;
}

CurPos move_left(char *buffer, size_t *buf_idx, CurPos pos) {
    if (*buf_idx != 0 && buffer[*buf_idx-1] != '\n') {
        *buf_idx -= 1;
        CurPos new_pos = {pos.y, pos.x-1, pos.x-1};
        return new_pos;
    }
    return pos;
}

void loop(char *buffer, size_t buf_size) {
    CurPos pos = {0, 0, 0};
    size_t buf_idx = 0;
    printw(buffer);
    refresh();
    while (1) {
        move(pos.y, pos.x);
        int key = getch();
        switch (key) {
            case KEY_UP:
                pos = move_up(buffer, &buf_idx, pos);
                break;
            case KEY_DOWN:
                pos = move_down(buffer, buf_size, &buf_idx, pos);
                break;
            case KEY_RIGHT:
                pos = move_right(buffer, buf_size, &buf_idx, pos);
                break;
            case KEY_LEFT:
                pos = move_left(buffer, &buf_idx, pos);
                break;
        }
        print_num(20, 20, buf_idx);
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
    loop(buffer, buf_size);

    free(buffer);
    endwin();
    return 0;
}
