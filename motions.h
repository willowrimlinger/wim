/**
 * @file motions.h
 * @author Max Rimlinger
 *
 * Header file for motions.c
 * Functions to move the cursor across the file
 */

#ifndef MOTIONS_H
#define MOTIONS_H

#include <stdlib.h>

#include "types.h"
#include "fileproxy.h"

View *move_up(FileProxy fp, View *view, const char mode);

View *move_down(FileProxy fp, View *view, const char mode);

View *move_left(FileProxy fp, View *view);

View *move_right(FileProxy fp, View *view, const char mode);

View *move_to_char(FileProxy fp, View *view, const char mode, const size_t ch);

View *move_to_eol(FileProxy fp, View *view, const char mode);

View *move_to_bol(FileProxy fp, View *view);

#endif

