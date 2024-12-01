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

#include "fileproxy.h"

/** Represents a cursor position in a file */
typedef struct Pos_s {
    size_t line;
    size_t ch;
    size_t desired_ch;
} Pos;

void move_cur(Pos pos);

Pos move_up(FileProxy fp, Pos pos);

Pos move_down(FileProxy fp, Pos pos);

Pos move_left(Pos pos);

Pos move_right(FileProxy fp, Pos pos);

#endif

