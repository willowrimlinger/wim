/**
 * @file motions.c
 * @author Max Rimlinger
 *
 * Functions to move the cursor across the file
 */

#include <stdlib.h>
#include <ncurses.h>

#include "types.h"
#include "motions.h"

void pan(View *view) {
    if (view->cur_line < view->top_line) {
        view->top_line = view->cur_line;
    } else if (view->cur_line > view->top_line + view->vlimit - 1) {
        view->top_line = view->cur_line - (view->vlimit - 1);
    }
    // handle scrolling horizontally when desired_ch is off screen
    if (view->cur_ch < view->left_ch) {
        view->left_ch = view->cur_ch;
    } else if (view->cur_ch > view->left_ch + view->hlimit - 1) {
        view->left_ch = view->cur_ch - (view->hlimit - 1);
    }
}

void move_up(FileProxy fp, View *view, const char mode) {
    if (view->cur_line == 0) {
        return;
    }

    view->cur_line -= 1;
    view->cur_ch = view->cur_desired_ch;

    size_t above_len = fp.lines[view->cur_line]->len;
    if (above_len <= view->cur_desired_ch) {
        if (mode == 'i') {
            view->cur_ch = above_len;
        } else {
            view->cur_ch = above_len - 1;
        }
    }
    if (above_len == 0) {
        view->cur_ch = 0;
    }

    pan(view);
}

void move_down(FileProxy fp, View *view, const char mode) {
    if (view->cur_line == fp.len - 1) {
        // can't move down, last line
        return;
    }

    view->cur_line += 1;
    view->cur_ch = view->cur_desired_ch;

    size_t below_len = fp.lines[view->cur_line]->len;
    if (below_len <= view->cur_desired_ch) {
        if (mode == 'i') {
            view->cur_ch = below_len;
        } else {
            view->cur_ch = below_len - 1;
        }
    }
    if (below_len == 0) {
        view->cur_ch = 0;
    }

    pan(view);
}

void move_left(FileProxy fp, View *view) {
    if (view->cur_ch == 0) {
        // can't move left, beginning of line
        return;
    }

    view->cur_ch -= 1;
    view->cur_desired_ch = view->cur_ch;

    pan(view);
}

void move_right(FileProxy fp, View *view, const char mode) {
    if (mode == 'i') {
        if (view->cur_ch == fp.lines[view->cur_line]->len) {
            // can't move right, end of line
            return;
        }
    } else {
        if (view->cur_ch == fp.lines[view->cur_line]->len - 1) {
            // can't move right, end of line
            return;
        }
    }

    view->cur_ch += 1;
    view->cur_desired_ch = view->cur_ch;

    pan(view);
}

/**
 * Move to a specific character number in the current line.
 *
 * @param fp the FileProxy to move in
 * @param view the current view before moving
 * @param ch the character number in the line
 * @return the new view after moving to the desired character
 */
void move_to_char(FileProxy fp, View *view, const char mode, const size_t ch) {
    Line *line = fp.lines[view->cur_line];
    if (mode == 'i') {
        if (ch >= line->len) {
            return;
        }
    } else {
        if (ch >= line->len - 1) {
            return;
        }
    }

    view->cur_ch = ch;
    view->cur_desired_ch = ch;

    pan(view);
}

void move_to_eol(FileProxy fp, View *view, const char mode) {
    size_t eol;
    if (mode == 'i') {
        eol = fp.lines[view->cur_line]->len;
    } else {
        eol = fp.lines[view->cur_line]->len - 1;
    }

    view->cur_ch = eol;
    view->cur_desired_ch = eol;

    pan(view);
}

void move_to_bol(FileProxy fp, View *view) {
    view->cur_ch = 0;
    view->cur_desired_ch = 0;

    pan(view);
}

