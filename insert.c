/**
 * @file insert.c
 * @author Max Rimlinger
 *
 * Contains functions for inserting and deleting text from the file
 */

#include <string.h>

#include "fileproxy.h"
#include "motions.h"

/**
 * Inserts one character into the file at the given cursor position and moves the
 * cursor over one.
 *
 * @param ch the character to insert
 * @param pos the current cursor position
 * @param fp the FileProxy to edit
 * @return the new cursor position after inserting
 */
Pos insert_char(char ch, FileProxy fp, Pos pos) {
    Line *line = fp.lines[pos.line];
    check_and_realloc_line(line);
    
    // move every char after ch over 1
    char *src = line->text + pos.ch; // cheeky pointer arithmetic
    char *dest = src + 1;
    memmove(dest, src, line->len - pos.ch + 1); // +1 for \0

    // insert char
    line->text[pos.ch] = ch;

    // update display

    // move cursor
    return move_right(fp, pos);
}
