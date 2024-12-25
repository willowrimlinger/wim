/**
 * @file insert.h
 * @author Max Rimlinger
 *
 * Conatins functions for inserting and deleting text from the file
 */

#include "types.h"
#include "fileproxy.h"
#include "motions.h"

#ifndef INSERT_H
#define INSERT_H

void insert_char(char ch, FileProxy *fp, View *view, MimState ms);

void backspace(FileProxy *fp, View *view, MimState ms);

void delete(FileProxy *fp, View *view, MimState ms);

void insert_newline(FileProxy *fp, View *view);

#endif
