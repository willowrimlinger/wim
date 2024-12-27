/**
 * @file motions.c
 * @author Max Rimlinger
 *
 * Functions to move the cursor across the file
 */

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "log.h"
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

void move_up(FileProxy fp, View *view, MimState ms) {
    if (view->cur_line == 0) {
        return;
    }

    view->cur_line -= 1;
    view->cur_ch = view->cur_desired_ch;

    size_t above_len = fp.lines[view->cur_line]->len;
    if (above_len <= view->cur_desired_ch) {
        if (ms.mode == INSERT) {
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

void move_down(FileProxy fp, View *view, MimState ms) {
    if (view->cur_line == fp.len - 1) {
        // can't move down, last line
        return;
    }

    view->cur_line += 1;
    view->cur_ch = view->cur_desired_ch;

    size_t below_len = fp.lines[view->cur_line]->len;
    if (below_len <= view->cur_desired_ch) {
        if (ms.mode == INSERT) {
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

void move_right(FileProxy fp, View *view, MimState ms) {
    if (ms.mode == INSERT) {
        if (view->cur_ch >= fp.lines[view->cur_line]->len) {
            // can't move right, end of line
            return;
        }
    } else {
        if (fp.lines[view->cur_line]->len == 0
                || view->cur_ch >= (fp.lines[view->cur_line]->len) - 1) {
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
void move_to_char(FileProxy fp, View *view, MimState ms, const size_t ch) {
    Line *line = fp.lines[view->cur_line];
    if (ms.mode == INSERT) {
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

void move_to_eol(FileProxy fp, View *view, MimState ms) {
    size_t eol;
    if (ms.mode == INSERT) {
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

void move_to_bof(FileProxy fp, View *view) {
    view->cur_line = 0;

    size_t beg_len = fp.lines[view->cur_line]->len;
    if (beg_len <= view->cur_desired_ch) {
        view->cur_ch = beg_len - 1;
    }
    if (beg_len == 0) {
        view->cur_ch = 0;
    }

    pan(view);
}

void move_to_eof(FileProxy fp, View *view) {
    view->cur_line = fp.lines[fp.len - 1]->num;

    size_t end_len = fp.lines[view->cur_line]->len;
    if (end_len <= view->cur_desired_ch) {
        view->cur_ch = end_len - 1;
    }
    if (end_len == 0) {
        view->cur_ch = 0;
    }

    pan(view);
}

