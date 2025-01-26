/**
 * @file insert.c
 * @author Max Rimlinger
 *
 * Functions for manipulating and executing commands in command mode
 */

#include <string.h>

#include "fileproxy.h"
#include "log.h"
#include "types.h"
#include "command.h"
#include "mode.h"

void type_command_char(MimState *ms, const char ch) {
    // TODO prolly refactor insert_char to take a line instead of fp
    int len = strlen(ms->command);
    if (len < MAX_COMMAND_LEN - 1) {
        ms->command[len] = ch;
        ms->command[len+1] = '\0';
    }
}

void backspace_command_char(MimState *ms) {
    int len = strlen(ms->command);
    if (len > 0) {
        ms->command[len-1] = '\0';
    }
}

void delete_command_char(MimState *ms) {

}

void exec_command(MimState *ms, FileProxy fp, View *view, const char *filename) {
    if (linecmp(ms->command, "w")) { write_fp(fp, filename);
    }
    switch_mode(fp, view, ms, NORMAL);
    // TODO message
}
