/**
 * @file display.c
 * @author Max Rimlinger
 *
 * Functions to display the program onto the ncurses window
 */

#include <string.h>
#include <ncurses.h>

#include "log.h"
#include "types.h"

const int TAB_WIDTH = 8;

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

/** 
 * Prints the contents of a FileProxy to the ncurses stdscr
 *
 * @param fp the FileProxy to print
 * @param view meta information about where the cursor is and where we are panned
 *      in the file
 */
void display_fp(FileProxy fp, View *view) {
    size_t rows_used = 0;
    for (size_t line = view->top_line; line < fp.len && rows_used < view->vlimit; line++) {
        size_t added_tab_width = 0;
        // see if this line will even fit
        for (size_t ch = 0; ch < fp.lines[line]->len; ch++) {
            const char ch = fp.lines[line]->text[ch];
            if (ch == '\t') {
                // calculate the number of spaces needed to get to the next tab
                added_tab_width += TAB_WIDTH - (ch + added_tab_width % TAB_WIDTH) - 1;
            }
        }
        size_t proposed_rows_used = (fp.lines[line]->len + added_tab_width) / view->hlimit + 1;
        if (proposed_rows_used + rows_used >= view->vlimit) {
            // line doesn't fit. we are done printing
            return;
        }
        
        added_tab_width = 0;
        for (size_t ch = 0; ch < fp.lines[line]->len; ch++) {
            const char ch = fp.lines[line]->text[ch];
            if (ch == '\t') {
                // calculate the number of spaces needed to get to the next tab
                added_tab_width += TAB_WIDTH - (ch + added_tab_width % TAB_WIDTH) - 1;
            } else {
                // print
                int row = rows_used + ((ch + added_tab_width) / view->hlimit + 1);
                int col = (ch + added_tab_width) % view->hlimit;
                mvaddch(row, col, ch);
            }
        }

        rows_used += proposed_rows_used;
        view->num_lines++;
    }
}

void display_status_bar(MimState ms) {
    // mode
    char *mode_str = "";
    if (ms.mode == INSERT) {
        mode_str = "-- INSERT --";
    }
    move(LINES - 1, 0);
    printw("%s", mode_str);
    
    // message
    // FIXME buncha weird stuff appears if you move around after a msg is displayed
    if (ms.status_msg != NULL) {
        move(LINES - 1, COLS - 1 - strlen(ms.status_msg));
        printw("%s", ms.status_msg);
    }
}

void display(MimState ms, FileProxy fp, View *view) {
    erase();

    display_fp(fp, view);
    display_status_bar(ms);

    refresh();
}

