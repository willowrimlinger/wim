/**
 * @file motions.c
 * @author Willow Rimlinger
 *
 * Functions to move the cursor across the file
 */

#include <stdlib.h>
#include <ncurses.h>

#include "types.h"
#include "motions.h"

void move_cur(View view) {
    move(view.cur_line - view.top_line, view.cur_ch - view.left_ch);
}

View move_up(FileProxy fp, View view) {
    if (view.cur_line == 0) {
        // can't move up, first line
        return view;
    }
    size_t above_len = fp.lines[view.cur_line-1]->len;
    View new_view = {
        view.top_line,
        view.left_ch,
        view.cur_line - 1,
        view.cur_desired_ch,
        view.cur_desired_ch
    };
    if (view.cur_line == view.top_line) {
        // scroll up
        new_view.top_line -= 1;
    }
    if (above_len <= view.cur_desired_ch) {
        new_view.cur_ch = above_len - 1;
    }
    if (above_len == 0) {
        new_view.cur_ch = 0;
    }
    // handle scrolling horizontally when desired_ch is off screen
    if (new_view.cur_ch < new_view.left_ch) {
        new_view.left_ch = new_view.cur_ch;
    } else if (new_view.cur_ch > new_view.left_ch + COLS - 1) {
        new_view.left_ch = new_view.cur_ch - (COLS - 1);
    }
    return new_view;
}

View move_down(FileProxy fp, View view) {
    if (view.cur_line == fp.len - 1) {
        // can't move down, last line
        return view;
    }
    size_t below_len = fp.lines[view.cur_line+1]->len;
    View new_view = {
        view.top_line,
        view.left_ch,
        view.cur_line+1,
        view.cur_desired_ch,
        view.cur_desired_ch
    };
    if (view.cur_line == view.top_line + LINES - 1) {
        // scroll down
        new_view.top_line += 1;
    }
    if (below_len <= view.cur_desired_ch) {
        new_view.cur_ch = below_len - 1;
    }
    if (below_len == 0) {
        new_view.cur_ch = 0;
    }
    // handle scrolling horizontally when desired_ch is off screen
    if (new_view.cur_ch < new_view.left_ch) {
        new_view.left_ch = new_view.cur_ch;
    } else if (new_view.cur_ch > new_view.left_ch + COLS - 1) {
        new_view.left_ch = new_view.cur_ch - (COLS - 1);
    }
    return new_view;
}

View move_left(FileProxy fp, View view) {
    if (view.cur_ch == 0) {
        // can't move left, beginning of line
        return view;
    }
    View new_view = {
        view.top_line,
        view.left_ch,
        view.cur_line,
        view.cur_ch - 1,
        view.cur_ch - 1
    };
    if (view.cur_ch == view.left_ch) {
        // scroll left
        new_view.left_ch -= 1;
    }
    return new_view;
}

View move_right(FileProxy fp, View view) {
    if (view.cur_ch == fp.lines[view.cur_line]->len - 1 ||
            fp.lines[view.cur_line]->len == 0) {
        // can't move right, end of line or empty line
        return view;
    }
    View new_view = {
        view.top_line,
        view.left_ch,
        view.cur_line,
        view.cur_ch + 1,
        view.cur_ch + 1
    };
    if (view.cur_ch == view.left_ch + COLS - 1) {
        // scroll right
        new_view.left_ch += 1;
    }
    return new_view;
}

