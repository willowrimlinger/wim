/**
 * @file display.c
 * @author Max Rimlinger
 *
 * Functions to display the program onto the ncurses window
 */

#include <ncurses.h>

#include "types.h"

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

void move_cur(View view) {
    move(view.cur_line - view.top_line, view.cur_ch - view.left_ch);
}

/** 
 * Prints the contents of a FileProxy to the ncurses stdscr
 *
 * @param fp the FileProxy to print
 * @param view meta information about where the cursor is and where we are panned
 *      in the file
 */
void display_fp(FileProxy fp, View view) {
    size_t line_limit = min(view.top_line + view.vlimit, fp.len);
    for (size_t i = view.top_line; i < line_limit; i++) {
        Line line = *fp.lines[i];
        size_t char_limit = min(view.left_ch + view.hlimit, line.len);
        for (size_t j = view.left_ch; j < char_limit; j++) {
            mvaddch(i - view.top_line, j - view.left_ch, line.text[j]);
        }
    }
}

void display(const char mode, FileProxy fp, View view) {
    erase();

    display_fp(fp, view);
    move_cur(view);

    refresh();
}

