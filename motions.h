/**
 * @file motions.h
 * @author Willow Rimlinger
 *
 * Header file for motions.c
 * Functions to move the cursor across the file
 */

#ifndef MOTIONS_H
#define MOTIONS_H

#include <stdlib.h>

#include "types.h"
#include "fileproxy.h"

void move_cur(View view);

View move_up(FileProxy fp, View view);

View move_down(FileProxy fp, View view);

View move_left(FileProxy fp, View view);

View move_right(FileProxy fp, View view);

#endif

