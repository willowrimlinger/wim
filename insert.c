/**
 * @file insert.c
 * @author Max Rimlinger
 *
 * Contains functions for inserting and deleting text from the file
 */

#include <string.h>

#include "types.h"
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
View insert_char(char ch, FileProxy fp, View view) {
    Line *line = fp.lines[view.cur_line];
    check_and_realloc_line(line);
    
    // move every char after ch over 1
    char *src = line->text + view.cur_ch; // cheeky pointer arithmetic
    char *dest = src + 1;
    memmove(dest, src, line->len - view.cur_ch + 1); // +1 for \0
    
    // update length
    line->len = line->len + 1;

    // insert char
    line->text[view.cur_ch] = ch;

    // move cursor
    return move_right(fp, view);
}
