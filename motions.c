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
#include "text_utils.h"
#include "text_objects.h"

void scroll_vertically(View *view) {
    if (view->cur.line < view->top_line) {
        view->top_line = view->cur.line;
    } else if (view->cur.line > view->top_line + view->num_lines - 1) {
        view->top_line = view->cur.line - (view->vlimit - 1);
    }
}

void move_up(FileProxy fp, View *view, MimState ms) {
    if (view->cur.line == 0) {
        return;
    }

    view->cur.line -= 1;
    view->cur.ch = view->cur_desired_ch;

    size_t above_len = fp.lines[view->cur.line]->len;
    if (above_len <= view->cur_desired_ch) {
        if (ms.mode == INSERT) {
            view->cur.ch = above_len;
        } else {
            view->cur.ch = above_len - 1;
        }
    }
    if (above_len == 0) {
        view->cur.ch = 0;
    }

    scroll_vertically(view);
}

void move_down(FileProxy fp, View *view, MimState ms) {
    if (view->cur.line == fp.len - 1) {
        // can't move down, last line
        return;
    }

    view->cur.line += 1;
    view->cur.ch = view->cur_desired_ch;

    size_t below_len = fp.lines[view->cur.line]->len;
    if (below_len <= view->cur_desired_ch) {
        if (ms.mode == INSERT) {
            view->cur.ch = below_len;
        } else {
            view->cur.ch = below_len - 1;
        }
    }
    if (below_len == 0) {
        view->cur.ch = 0;
    }

    scroll_vertically(view);
}

void move_left(FileProxy fp, View *view) {
    if (view->cur.ch == 0) {
        // can't move left, beginning of line
        return;
    }

    view->cur.ch -= 1;
    view->cur_desired_ch = view->cur.ch;

    scroll_vertically(view);
}

void move_right(FileProxy fp, View *view, MimState ms) {
    if (ms.mode == INSERT) {
        if (view->cur.ch >= fp.lines[view->cur.line]->len) {
            // can't move right, end of line
            return;
        }
    } else {
        if (fp.lines[view->cur.line]->len == 0
                || view->cur.ch >= (fp.lines[view->cur.line]->len) - 1) {
            // can't move right, end of line
            return;
        }
    }

    view->cur.ch += 1;
    view->cur_desired_ch = view->cur.ch;

    scroll_vertically(view);
}

void move_to_line(FileProxy fp, View *view, MimState ms, const size_t line) {
    view->cur.line = line;
    view->cur.ch = view->cur_desired_ch;

    size_t line_len = fp.lines[view->cur.line]->len;
    if (line_len <= view->cur_desired_ch) {
        if (ms.mode == INSERT) {
            view->cur.ch = line_len;
        } else {
            view->cur.ch = line_len - 1;
        }
    }
    if (line_len == 0) {
        view->cur.ch = 0;
    }

    scroll_vertically(view);
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
    Line *line = fp.lines[view->cur.line];
    if (ms.mode == INSERT) {
        if (ch >= line->len) {
            return;
        }
    } else {
        if (ch >= line->len - 1) {
            return;
        }
    }

    view->cur.ch = ch;
    view->cur_desired_ch = ch;

    scroll_vertically(view);
}

void move_to_eol(FileProxy fp, View *view, MimState ms) {
    size_t eol;
    if (ms.mode == INSERT) {
        eol = fp.lines[view->cur.line]->len;
    } else {
        eol = fp.lines[view->cur.line]->len - 1;
    }

    view->cur.ch = eol;
    view->cur_desired_ch = eol;

    scroll_vertically(view);
}

void move_to_bol(FileProxy fp, View *view) {
    view->cur.ch = 0;
    view->cur_desired_ch = 0;

    scroll_vertically(view);
}

void move_to_bol_non_ws(FileProxy fp, View *view, MimState ms) {
    Line *line = fp.lines[view->cur.line];

    size_t ch_idx = get_len_ws_beginning(*line);
    if (ms.mode == INSERT) {
        if (line->len > 0) {
            if (ch_idx == line->len - 1) {
                ch_idx++;
            }
        }
    }

    move_to_char(fp, view, ms, ch_idx);

    scroll_vertically(view);
}

void move_to_bof(FileProxy fp, View *view) {
    view->cur.line = 0;

    size_t beg_len = fp.lines[view->cur.line]->len;
    if (beg_len <= view->cur_desired_ch) {
        view->cur.ch = beg_len - 1;
    }
    if (beg_len == 0) {
        view->cur.ch = 0;
    }

    scroll_vertically(view);
}

void move_to_eof(FileProxy fp, View *view) {
    view->cur.line = fp.lines[fp.len - 1]->num;

    size_t end_len = fp.lines[view->cur.line]->len;
    if (end_len <= view->cur_desired_ch) {
        view->cur.ch = end_len - 1;
    }
    if (end_len == 0) {
        view->cur.ch = 0;
    }

    scroll_vertically(view);
}

void move_to_beg_n_tobj(FileProxy fp, View *view, TextObject tobj) {
    CurPos beg_n_tobj_pos = get_beg_pos_n_tobj(fp, view->cur, tobj);
    view->cur.line = beg_n_tobj_pos.line;
    view->cur.ch = beg_n_tobj_pos.ch;

    scroll_vertically(view);
}

void move_to_beg_p_tobj(FileProxy fp, View *view, TextObject tobj) {
    CurPos beg_p_tobj_pos = get_beg_pos_p_tobj(fp, view->cur, tobj);
    view->cur.line = beg_p_tobj_pos.line;
    view->cur.ch = beg_p_tobj_pos.ch;

    scroll_vertically(view);
}

