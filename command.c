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
    int len = strlen(ms->command);
    if (len < MAX_COMMAND_LEN - 1) {
        ms->command[len] = ch;
        ms->command[len+1] = '\0';
    }
}

void exec_command(MimState *ms, FileProxy fp, View *view, const char *filename) {
    log_to_file("cmd: \"%s\" len: %lu", ms->command, strlen(ms->command));
    if (strcmp(ms->command, "w") == 0) {
        log_to_file("writing");
        write_fp(fp, filename);
    }
    switch_mode(fp, view, ms, NORMAL);
    // TODO message
}
