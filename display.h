/**
 * @file display.h
 * @author Max Rimlinger
 *
 * Functions to display the program onto the ncurses window
 */

#include "types.h"

static const size_t MAX_STATUS_MSG_LEN = 200;

void display(MimState ms, FileProxy fp, View view);

