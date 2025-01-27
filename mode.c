/**
 * @file mode.c
 * @author Max Rimlinger
 *
 * Functions for changing modes that will handle the side effects of doing so
 */

#include <ncurses.h>

#include "fileproxy.h"
#include "types.h"
#include "motions.h"
#include "log.h"

void clear_status_msg(MimState *ms) {
    ms->status_msg[0] = '\0';
}

void switch_to_insert_mode(MimState *ms) {
    ms->mode = INSERT;
}

void switch_to_normal_mode(FileProxy fp, View *view, MimState *ms) {
    ms->mode = NORMAL;

    // handle cursor past the end of a line in insert mode
    Line *cur_line = fp.lines[view->cur.line];
    if (cur_line->len > 0) {
        move_left(fp, view);
    }
}

void switch_to_command_mode(MimState *ms) {
    ms->mode = COMMAND;
}

void switch_from_command_mode(MimState *ms) {
    // clear line
    Line *line = ms->cmd_fp->lines[0];
    free(line->text);
    free(line);
    line = create_line(0);

    // clear view
    ms->cmd_view->top_line = 0;
    ms->cmd_view->left_ch = 0;
    ms->cmd_view->vlimit = 1;
    ms->cmd_view->hlimit = COLS - 1;
    ms->cmd_view->cur.ch = 0;
    ms->cmd_view->cur.line = 0;
    ms->cmd_view->cur_desired_ch = 0;
}

void switch_mode(FileProxy fp, View *view, MimState *ms, Mode new_mode) {
    switch (ms->mode) {
        case INSERT:
        case NORMAL:
            break;
        case COMMAND:
            switch_from_command_mode(ms);
            break;
        
    }
    switch (new_mode) {
        case INSERT:
            switch_to_insert_mode(ms);
            break;
        case NORMAL:
            switch_to_normal_mode(fp, view, ms);
            break;
        case COMMAND:
            switch_to_command_mode(ms);
            break;
    }
    clear_status_msg(ms);
}

