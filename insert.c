/**
 * @file insert.c
 * @author Max Rimlinger
 *
 * Contains functions for inserting and deleting text from the file
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

// FIXME may or may not work
View delete_line(FileProxy *fp, View view) {
    if (view.cur_line == 0) {
        return view;
    }

    // move subsequent lines up one
    Line **src = fp->lines + view.cur_line + 1;
    Line **dest = fp->lines + view.cur_line;
    memmove(dest, src, (fp->len - (view.cur_line + 1)) * sizeof(Line *));

    // update length
    fp->len -= 1;

    // update the line numbers of subsequent lines
    for (size_t i = view.cur_line; i < fp->len; i++) {
        fp->lines[i]->num -= 1;
    }

    // shorten lines array
    Line **tmp = realloc(fp->lines,  fp->len * sizeof(Line *));
    if (tmp != NULL) {
        fp->lines = tmp;
    } else {
        fprintf(stderr, "Error reallocating space for new line.\n");
        exit(EXIT_FAILURE);
    }

    return move_up(*fp, view);
}

/**
 * Appends the text of the current line to the previous line with no space.
 * Moves the cursor to the previous line where the newly inserted text begins.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new cursor position after combining
 */
View combine_line_with_prev(FileProxy *fp, View view) {
    if (view.cur_line == 0) {
        return view;
    }

    // move text
    Line *cur_line = fp->lines[view.cur_line];
    Line *prev_line = fp->lines[view.cur_line-1];
    size_t prev_line_len_before_combining = prev_line->len;
    if (cur_line->len > 0) {
        // add the text from the current line to the prev line
        check_and_realloc_line(prev_line, cur_line->len);
        strcpy(prev_line->text + prev_line->len, cur_line->text);
        prev_line->len += cur_line->len;
    }

    // move subsequent lines up one
    Line **src = fp->lines + view.cur_line + 1;
    Line **dest = fp->lines + view.cur_line;
    memmove(dest, src, (fp->len - (view.cur_line + 1)) * sizeof(Line *));

    // update length
    fp->len -= 1;

    // update the line numbers of subsequent lines
    for (size_t i = view.cur_line; i < fp->len; i++) {
        fp->lines[i]->num -= 1;
    }

    // shorten lines array
    Line **tmp = realloc(fp->lines,  fp->len * sizeof(Line *));
    if (tmp != NULL) {
        fp->lines = tmp;
    } else {
        fprintf(stderr, "Error reallocating space for new line.\n");
        exit(EXIT_FAILURE);
    }

    return move_to_char(*fp, move_up(*fp, view), prev_line_len_before_combining);
}

/**
 * Appends the text of the next line to the end of the current line.
 * Moves the cursor to where the newly inserted text begins on the current line.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new cursor position after combining
 */
View combine_line_with_next(FileProxy *fp, View view) {
    if (view.cur_line == fp->len - 1) {
        return view;
    }

    // move text
    Line *cur_line = fp->lines[view.cur_line];
    Line *next_line = fp->lines[view.cur_line+1];
    size_t cur_line_len_before_combining = cur_line->len;
    if (next_line->len > 0) {
        // add the text from the next line to the current line
        check_and_realloc_line(cur_line, next_line->len);
        strcpy(cur_line->text + cur_line->len, next_line->text);
        cur_line->len += next_line->len;
    }

    // move lines after next line up one
    if (view.cur_line < fp->len - 2) {
        Line **src = fp->lines + view.cur_line + 2;
        Line **dest = fp->lines + view.cur_line + 1;
        memmove(dest, src, (fp->len - (view.cur_line + 2)) * sizeof(Line *));
    }

    // update length
    fp->len -= 1;

    // update the line numbers of subsequent lines
    for (size_t i = view.cur_line + 1; i < fp->len; i++) {
        fp->lines[i]->num -= 1;
    }

    // shorten lines array
    Line **tmp = realloc(fp->lines,  fp->len * sizeof(Line *));
    if (tmp != NULL) {
        fp->lines = tmp;
    } else {
        fprintf(stderr, "Error reallocating space for new line.\n");
        exit(EXIT_FAILURE);
    }

    return move_to_char(*fp, view, cur_line_len_before_combining);
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
        return combine_line_with_prev(fp, view);
    }

    Line *line = fp->lines[view.cur_line];
    check_and_realloc_line(line, -1);

    // move every char from cursor onwards left 1
    char *src = line->text + view.cur_ch;
    char *dest = src - 1;
    memmove(dest, src, (line->len - view.cur_ch + 1) * byte); // +1 for \0
    
    // update length
    line->len -= 1;
    
    // move cursor
    return move_left(*fp, view);
}

View delete(FileProxy *fp, View view) {
    Line *line = fp->lines[view.cur_line];
    if (view.cur_ch == line->len) {
        return combine_line_with_next(fp, view);
    }

    check_and_realloc_line(line, - 1);

    // move every char from char after cursor onwards left 1
    char *src = line->text + view.cur_ch + 1;
    char *dest = src - 1;
    // +1 for converting 0-indexed cur_ch to 1-indexed len, +1 for \0
    memmove(dest, src, (line->len - (view.cur_ch + 1) + 1) * byte); 
    
    // update length
    line->len -= 1;

    return view;
}

/**
 * Inserts a new Line into the FileProxy after the current line.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new view with the cursor at the beginning of the new line
 */
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

    // move text
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
    cur_line->text[cur_line->len] = '\0';

    // move to new line
    return move_to_bol(*fp, move_down(*fp, view));
}

