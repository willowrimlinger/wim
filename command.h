/**
 * @file command.h
 * @author Max Rimlinger
 *
 * Functions for manipulating and executing commands in command mode
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "types.h"

static const int MAX_COMMAND_LEN = 512; // bad design. should be arbitrary len but i'm lazy

void type_command_char(MimState *ms, const char ch);

void exec_command(MimState *ms, FileProxy fp, View *view, const char *filename);

#endif
