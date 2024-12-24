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

View *insert_char(char ch, FileProxy *fp, View *view, const char mode);

View *backspace(FileProxy *fp, View *view, const char mode);

View *delete(FileProxy *fp, View *view, const char mode);

View *insert_newline(FileProxy *fp, View *view);

#endif
