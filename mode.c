/**
 * @file mode.c
 * @author Max Rimlinger
 *
 * Functions for changing modes that will handle the side effects of doing so
 */

#include <ncurses.h>

#include "types.h"
#include "motions.h"

void switch_to_insert_mode(FileProxy fp, View *view, MimState *ms) {
    ms->mode = INSERT;
}

void switch_to_normal_mode(FileProxy fp, View *view, MimState *ms) {
    ms->mode = NORMAL;

    // handle cursor past the end of a line in insert mode
    Line *cur_line = fp.lines[view->cur_line];
    if (cur_line->len > 0) {
        move_left(fp, view);
    }
}

void switch_mode(FileProxy fp, View *view, MimState *ms, Mode new_mode) {
    switch (new_mode) {
        case INSERT:
            switch_to_insert_mode(fp, view, ms);
            break;
        case NORMAL:
            switch_to_normal_mode(fp, view, ms);
            break;
    }
}

