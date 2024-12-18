/**
 * @file insert.c
 * @author Max Rimlinger
 *
 * Contains functions for inserting and deleting text from the file
 */

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "fileproxy.h"
#include "motions.h"
#include "log.h"

static const size_t byte = sizeof(unsigned char);

/**
 * Inserts one character into the file at the given cursor position and moves the
 * cursor over one.
 *
 * @param ch the character to insert
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new cursor position after inserting
 */
View insert_char(char ch, FileProxy *fp, View view) {
    Line *line = fp->lines[view.cur_line];
    check_and_realloc_line(line, 1);
    
    // move every char after ch over 1
    char *src = line->text + view.cur_ch; // cheeky pointer arithmetic
    char *dest = src + 1;
    memmove(dest, src, (line->len - view.cur_ch + 1) * byte); // +1 for \0
    
    // update length
    line->len += 1;

    // insert char
    line->text[view.cur_ch] = ch;
    
    // move cursor
    return move_right(*fp, view);
}

/**
 * Deletes the character before the cursor position. The cursor moves left one
 * to stay with the character it was on.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new view with the cursor moved one left
 */
View backspace(FileProxy *fp, View view) {
    if (view.cur_ch == 0) {
        return view;
    }

    Line *line = fp->lines[view.cur_line];
    check_and_realloc_line(line, -1);

    // move every char from cursor onwards left 1
    char *src = line->text + view.cur_ch;
    char *dest = src - 1;
    memmove(dest, src, (line->len - view.cur_ch + 1) * byte);
    
    // update length
    line->len -= 1;
    
    // move cursor
    return move_left(*fp, view);
}

View insert_newline(FileProxy *fp, View view) {
    // make space for a new line
    Line **tmp = realloc(fp->lines,  (fp->len + 1) * sizeof(Line *));
    if (tmp != NULL) {
        fp->lines = tmp;
    } else {
        fprintf(stderr, "Error reallocating space for new line.\n");
        exit(EXIT_FAILURE);
    }

    // if we aren't on the last line
    if (view.cur_line != fp->len - 1) {
        // move every line after the current down 1
        Line **src = fp->lines + view.cur_line + 1;
        Line **dest = fp->lines + view.cur_line + 2;
        memmove(dest, src, (fp->len - (view.cur_line + 1)) * sizeof(Line *));
    }

    // update length
    fp->len += 1;

    // insert new line
    fp->lines[view.cur_line+1] = create_line(view.cur_line + 1);

    // update the line numbers of each subsequent line
    for (size_t i = view.cur_line + 2; i < fp->len; i++) {
        fp->lines[i]->num += 1;
    }

    Line *cur_line = fp->lines[view.cur_line];
    Line *new_line = fp->lines[view.cur_line+1];
    size_t text_to_eol_len = cur_line->len - view.cur_ch;
    // add the text from cursor to eol to the new line
    check_and_realloc_line(new_line, text_to_eol_len);
    strcpy(new_line->text, cur_line->text + view.cur_ch);
    new_line->len += text_to_eol_len;
    // remove the text from cursor to eol
    check_and_realloc_line(cur_line, -text_to_eol_len);
    cur_line->len -= text_to_eol_len;

    // move to new line
    return move_to_bol(*fp, move_down(*fp, view));
}

