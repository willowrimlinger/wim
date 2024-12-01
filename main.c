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

const size_t BUF_INCR = 16;

const int NORMAL_KEYS_LEN = 96;
const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>?";

typedef struct Line_s {
    char *text;
    size_t len;
    size_t cap;
} Line;

typedef struct FileProxy_s {
    Line **lines;
    size_t len;
} FileProxy;

void print_num(int y, int x, int num) { //FIXME remove
        move(y, x);
        char str[3];
        sprintf(str, "%d", num);
        printw(str);
}

FileProxy split_buffer(char *buffer, size_t buf_len) {
    size_t num_lines = 1;
    for (size_t i = 0; i < buf_len; i++) {
        if (buffer[i] == '\n' && i != buf_len - 1) {
            num_lines++;
        }
    }
    Line **lines = malloc(sizeof(Line *) * num_lines);
    size_t lines_idx = 0;

    // create the first line
    Line *line = malloc(sizeof(Line));
    size_t byte = sizeof(unsigned char);
    char *text = malloc(BUF_INCR * byte + 1); // +1 for terminating null byte
    Line new_line = {text, 0, BUF_INCR};
    *line = new_line;
    lines[lines_idx] = line;

    for (size_t i = 0; i < buf_len - 1; i++) {
        // create a new line
        if (buffer[i] == '\n') {
            Line *line = malloc(sizeof(Line));
            char *text = malloc(BUF_INCR * byte + 1); // +1 for terminating null byte
            Line new_line = {text, 0, BUF_INCR};
            *line = new_line;
            lines_idx++;
            lines[lines_idx] = line;
        } else {
            // increase the text buffer size of the line if it is full
            size_t text_len = lines[lines_idx]->len;
            size_t cap = lines[lines_idx]->cap;
            size_t byte = sizeof(unsigned char);
            if (text_len == cap) {
                char *tmp = realloc(lines[lines_idx]->text, text_len + (BUF_INCR * byte) + 1);
                if (tmp != NULL) {
                    lines[lines_idx]->text = tmp;
                } else {
                    perror("Error reallocating space for line text");
                    exit(EXIT_FAILURE);
                }
                lines[lines_idx]->cap = text_len + BUF_INCR;
            }

            // add a character on to the line
            lines[lines_idx]->text[text_len] = buffer[i];
            lines[lines_idx]->text[text_len+1] = '\0';
            lines[lines_idx]->len = text_len + 1;
        }
    }
    FileProxy fp = {lines, num_lines};
    return fp;
}

void free_fp(FileProxy fp) {
    for (size_t i = 0; i < fp.len; i++) {
        free(fp.lines[i]->text);
        fp.lines[i]->text = NULL;
        free(fp.lines[i]);
        fp.lines[i] = NULL;
    }
    free(fp.lines);
    fp.lines = NULL;
}

void print_fp(FileProxy fp) {
    for (size_t i = 0; i < fp.len; i++) {
        Line line = *fp.lines[i];
        printf("len: %lu\ncap: %lu\n", line.len, line.cap);
        for (size_t j = 0; j < line.len; j++) {
            if (line.text[j] == '\n') {
                printf("newline");
            } else {
                putchar(line.text[j]);
            }
        }
        putchar('\n');
    }
}

void loop(char *buffer, size_t buf_size) {
    printw(buffer);
    refresh();
    while (1) {
//        move(pos.y, pos.x);
        int key = getch();
        switch (key) {
            case KEY_UP:
//                pos = move_up(buffer, &buf_idx, pos);
                break;
            case KEY_DOWN:
//                pos = move_down(buffer, buf_size, &buf_idx, pos);
                break;
            case KEY_RIGHT:
//                pos = move_right(buffer, buf_size, &buf_idx, pos);
                break;
            case KEY_LEFT:
//                pos = move_left(buffer, &buf_idx, pos);
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
    free_fp(fp);

//    initscr();
//    keypad(stdscr, TRUE);

    // main program loop
//    loop(buffer, buf_size);

//    endwin();
    return 0;
}
