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

static const size_t byte = sizeof(unsigned char);

/**
 * Inserts one character into the file at the given cursor position and moves the
 * cursor over one.
 *
 * @param ch the character to insert
 * @param pos the current cursor position
 * @param fp the FileProxy to edit
 * @return the new cursor position after inserting
 */
View insert_char(char ch, FileProxy *fp, View view) {
    Line *line = fp->lines[view.cur_line];
    check_and_realloc_line(line, 1);
    
    // move every char after ch over 1
    char *src = line->text + view.cur_ch; // cheeky pointer arithmetic
    char *dest = src + 1;
    memmove(dest, src, line->len - (view.cur_ch * byte) + 1); // +1 for \0
    
    // update length
    line->len += 1;

    // insert char
    line->text[view.cur_ch] = ch;

    // move cursor
    return move_right(*fp, view);
}

View insert_newline(FileProxy *fp, View view) {
    // move every line after the current down 1
    Line *src = *(fp->lines + view.cur_line + 1);
    Line *dest = src + 1;
    memmove(dest, src, (fp->len - (view.cur_line + 1)) * sizeof(Line *));

    // update length
    fp->len += 1;

    // insert new line
    fp->lines[view.cur_line+1] = create_line(view.cur_line + 1);

    // update the line numbers of each subsequent line
    for (size_t i = view.cur_line + 2; i < fp->len; i++) {
        fp->lines[i]->num += 1;
    }

    size_t text_to_eol_len = fp->lines[view.cur_line]->len - view.cur_ch;
    // add the text from cursor to eol to the new line
    check_and_realloc_line(fp->lines[view.cur_line+1], text_to_eol_len);
    strcpy(fp->lines[view.cur_line+1]->text, fp->lines[view.cur_line]->text + view.cur_ch);
    // remove the text from cursor to eol
    check_and_realloc_line(fp->lines[view.cur_line], -text_to_eol_len);
    // move to new line
    return move_down(*fp, view);
}

