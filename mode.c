/**
 * @file mode.c
 * @author Max Rimlinger
 *
 * Functions for changing modes that will handle the side effects of doing so
 */

#include <ncurses.h>

#include "types.h"
#include "motions.h"
#include "log.h"

static const size_t COMMAND_BUF_INCR = 16;

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
    ms->command[0] = '\0';
}

void switch_mode(FileProxy fp, View *view, MimState *ms, Mode new_mode) {
    switch (ms->mode) {
        case INSERT:
        case NORMAL:
            break;
        case COMMAND:
            log_to_file("Coming from cmd");
            switch_from_command_mode(ms);
            break;
        
    }
    switch (new_mode) {
        case INSERT:
            log_to_file("Switching to insert");
            switch_to_insert_mode(ms);
            break;
        case NORMAL:
            log_to_file("Switching to normal");
            switch_to_normal_mode(fp, view, ms);
            break;
        case COMMAND:
            log_to_file("Switching to command");
            switch_to_command_mode(ms);
            break;
    }
}

