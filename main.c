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
#include "mode.h"
#include "fileproxy.h"
#include "motions.h"
#include "insert.h"
#include "display.h"
#include "command.h"

static const char *NORMAL_KEYS = "`~1!2@3#4$5%6^7&8*9(0)-_=+qwertyuiop[]\\QWERTYUIOP{}|asdfghjkl;'ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>? ";

static void loop(FileProxy fp, const char *filename) {
    View view = {0, 0, LINES - 1, COLS, 0, 0, 0};
    Line *command = create_line(0);
    MimState ms = {NULL, command, NORMAL};
    switch_mode(fp, &view, &ms, NORMAL);
    while (1) {
        display(ms, fp, view);
        int key = getch();
        switch (ms.mode) {
            case INSERT:
                switch (key) {
                    case KEY_UP:
                        move_up(fp, &view, ms);
                        break;
                    case KEY_DOWN:
                        move_down(fp, &view, ms);
                        break;
                    case KEY_LEFT:
                        move_left(fp, &view);
                        break;
                    case KEY_RIGHT:
                        move_right(fp, &view, ms);
                        break;
                    case KEY_END:
                        move_to_eol(fp, &view, ms);
                        break;
                    case KEY_HOME:
                        move_to_bol(fp, &view);
                        break;
                    case KEY_ENTER:
                    case '\n':
                    case '\r':
                        insert_newline(&fp, &view, ms);
                        break;
                    case KEY_BACKSPACE:
                        backspace(&fp, &view, ms);
                        break;
                    case KEY_DC:
                        delete_char(&fp, &view, ms);
                        break;
                    case 27:
                        switch_mode(fp, &view, &ms, NORMAL);
                        break;
                }
                // text insertion
                for (int i = 0; NORMAL_KEYS[i] != '\0'; i++) {
                    if (key == NORMAL_KEYS[i]) {
                        insert_char(key, &fp, &view, ms);
                    }
                }
                break;
            case NORMAL:
                switch (key) {
                    case KEY_UP:
                    case 'k':
                        move_up(fp, &view, ms);
                        break;
                    case KEY_DOWN:
                    case 'j':
                        move_down(fp, &view, ms);
                        break;
                    case KEY_LEFT:
                    case 'h':
                        move_left(fp, &view);
                        break;
                    case KEY_RIGHT:
                    case 'l':
                        move_right(fp, &view, ms);
                        break;
                    case KEY_END:
                    case '$':
                        move_to_eol(fp, &view, ms);
                        break;
                    case KEY_HOME:
                    case '0':
                        move_to_bol(fp, &view);
                        break;
                    case '^':
                        move_to_bol_non_ws(fp, &view, ms);
                        break;
                    case 'G':
                        move_to_eof(fp, &view);
                        break;
                    case 'g':
                        {
                            int key2 = getch();
                            switch (key2) {
                                case 'g':
                                    move_to_bof(fp, &view);
                                    break;
                                break;
                            }
                        }
                        break;
                    case KEY_ENTER:
                    case '\n':
                    case '\r':
                        move_down(fp, &view, ms);
                        move_to_bol_non_ws(fp, &view, ms);
                        break;
                    case KEY_BACKSPACE:
                        // TODO move left, or move up a line if on first char
                        break;
                    case KEY_DC:
                    case 'x':
                        delete_char(&fp, &view, ms);
                        break;
                    case 'i':
                        switch_mode(fp, &view, &ms, INSERT);
                        break;
                    case 'a':
                        switch_mode(fp, &view, &ms, INSERT);
                        move_right(fp, &view, ms);
                        break;
                    case 'A':
                        switch_mode(fp, &view, &ms, INSERT);
                        move_to_eol(fp, &view, ms);
                        break;
                    case 'o':
                        switch_mode(fp, &view, &ms, INSERT);
                        move_to_eol(fp, &view, ms);
                        insert_newline(&fp, &view, ms);
                        break;
                    case 'w':
                        move_to_beg_n_tobj(fp, &view, WORD);
                        break;
                    case 'b':
                        move_to_beg_p_tobj(fp, &view, WORD);
                        break;
                    case ':':
                        log_to_file("pressed :");
                        switch_mode(fp, &view, &ms, COMMAND);
                        break;
                }
                    break;
            case COMMAND:
                switch (key) {
                    case KEY_ENTER:
                    case '\n':
                    case '\r':
                        exec_command(&ms, fp, &view, filename);
                        break;
                    case KEY_BACKSPACE:
                        // TODO backspace
                        break;
                    case KEY_DC:
                        // TODO delete
                        break;
                    case KEY_END:
                        // TODO
                        break;
                    case KEY_HOME:
                        // TODO
                        break;
                }
                // text insertion
                for (int i = 0; NORMAL_KEYS[i] != '\0'; i++) {
                    if (key == NORMAL_KEYS[i]) {
                        type_command_char(&ms, key);
                    }
                }
                break;
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
