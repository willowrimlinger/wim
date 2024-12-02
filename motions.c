/**
 * @file motions.c
 * @author Max Rimlinger
 *
 * Functions to move the cursor across the file
 */

#include <stdlib.h>
#include <ncurses.h>

#include "motions.h"

void move_cur(Pos pos) {
    // TODO text wrapping, text overflow
    move(pos.line, pos.ch);
}

Pos move_up(FileProxy fp, Pos pos) {
    if (pos.line == 0) {
        // can't move up, first line
        return pos;
    }
    size_t above_len = fp.lines[pos.line-1]->len;
    Pos new_pos = {pos.line - 1, pos.desired_ch, pos.desired_ch};
    if (above_len <= pos.desired_ch) {
        new_pos.ch = above_len - 1;
    }
    if (above_len == 0) {
        new_pos.ch = 0;
    }
    return new_pos;
}

Pos move_down(FileProxy fp, Pos pos) {
    if (pos.line == fp.len - 1) {
        // can't move down, last line
        return pos;
    }
    size_t below_len = fp.lines[pos.line+1]->len;
    Pos new_pos = {pos.line+1, pos.desired_ch, pos.desired_ch};
    if (below_len <= pos.desired_ch) {
        new_pos.ch = below_len - 1;
    }
    if (below_len == 0) {
        new_pos.ch = 0;
    }
    return new_pos;
}

Pos move_left(Pos pos) {
    if (pos.ch == 0) {
        // can't move left, beginning of line
        return pos;
    }
    Pos new_pos = {pos.line, pos.ch - 1, pos.ch - 1};
    return new_pos;
}

Pos move_right(FileProxy fp, Pos pos) {
    if (pos.ch == fp.lines[pos.line]->len - 1 ||
            fp.lines[pos.line]->len == 0) {
        // can't move right, end of line or empty line
        return pos;
    }
    Pos new_pos = {pos.line, pos.ch + 1, pos.ch + 1};
    return new_pos;
}

