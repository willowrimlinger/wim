/**
 * @file command.h
 * @author Max Rimlinger
 *
 * Functions for manipulating and executing commands in command mode
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "types.h"

void exec_command(MimState *ms, FileProxy fp, View *view, const char *filename);

#endif
