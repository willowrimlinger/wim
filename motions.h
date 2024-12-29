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

void move_up(FileProxy fp, View *view, MimState ms);

void move_down(FileProxy fp, View *view, MimState ms);

void move_left(FileProxy fp, View *view);

void move_right(FileProxy fp, View *view, MimState ms);

void move_to_char(FileProxy fp, View *view, MimState ms, const size_t ch);

void move_to_eol(FileProxy fp, View *view, MimState ms);

void move_to_bol(FileProxy fp, View *view);

void move_to_bol_non_ws(FileProxy fp, View *view, MimState ms);

void move_to_bof(FileProxy fp, View *view);

void move_to_eof(FileProxy fp, View *view);

void move_to_beg_n_tobj(FileProxy fp, View *view, TextObject tobj);

#endif

