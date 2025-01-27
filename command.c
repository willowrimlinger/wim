/**
 * @file command.c
 * @author Max Rimlinger
 *
 * Functions for manipulating and executing commands in command mode
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "fileproxy.h"
#include "log.h"
#include "types.h"
#include "command.h"
#include "mode.h"
#include "insert.h"
#include "display.h"

bool exec_command(MimState *ms, FileProxy fp, View *view, const char *filename) {
    char status_msg[MAX_STATUS_MSG_LEN];
    if (linecmp(ms->cmd_fp->lines[0], "w")) {
        size_t size = write_fp(fp, filename);
        sprintf(status_msg, "\"%s\" %luL, %luB written", filename, fp.len, size);
    } else if (linecmp(ms->cmd_fp->lines[0], "q")) {
        return false;
    } else if (linecmp(ms->cmd_fp->lines[0], "wq")) {
        write_fp(fp, filename);
        return false;
    }
    switch_mode(fp, view, ms, NORMAL);
    strcpy(ms->status_msg, status_msg);
    return true;
}
