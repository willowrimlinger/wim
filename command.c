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
#include "insert.h"

void exec_command(MimState *ms, FileProxy fp, View *view, const char *filename) {
    if (linecmp(ms->cmd_fp->lines[0], "w")) {
        write_fp(fp, filename);
    }
    switch_mode(fp, view, ms, NORMAL);
    // TODO message
}
