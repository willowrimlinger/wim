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
#include "text_utils.h"

static const size_t byte = sizeof(unsigned char);

/**
 * Inserts one character into the file at the current cursor position and moves the
 * cursor over one.
 *
 * @param ch the character to insert
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new cursor position after inserting
 */
void insert_char(char ch, FileProxy *fp, View *view, MimState ms) {
    Line *line = fp->lines[view->cur.line];
    check_and_realloc_line(line, 1);
    
    // move every char after ch over 1
    char *src = line->text + view->cur.ch; // cheeky pointer arithmetic
    char *dest = src + 1;
    memmove(dest, src, (line->len - view->cur.ch + 1) * byte); // +1 for \0
    
    // update length
    line->len += 1;

    // insert char
    line->text[view->cur.ch] = ch;
    log_to_file("i be inserting a char");
    log_to_file("line len: lu", line->len);
    log_to_file("contents:");
    for (size_t i = 0; i < line->len; i++) {
        log_to_file("%c", line->text[i]);
    }
    // move cursor
    move_right(*fp, view, ms);
}

// FIXME may or may not work
void delete_line(FileProxy *fp, View *view, MimState ms) {
    if (view->cur.line == 0) {
        return;
    }

    // move subsequent lines up one
    Line **src = fp->lines + view->cur.line + 1;
    Line **dest = fp->lines + view->cur.line;
    memmove(dest, src, (fp->len - (view->cur.line + 1)) * sizeof(Line *));

    // update length
    fp->len -= 1;

    // update the line numbers of subsequent lines
    for (size_t i = view->cur.line; i < fp->len; i++) {
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

    move_up(*fp, view, ms);
}

/**
 * Appends the text of the current line to the previous line with no space.
 * Moves the cursor to the previous line where the newly inserted text begins.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new cursor position after combining
 */
void combine_line_with_prev(FileProxy *fp, View *view, MimState ms) {
    if (view->cur.line == 0) {
        return;
    }

    // move text
    Line *cur_line = fp->lines[view->cur.line];
    Line *prev_line = fp->lines[view->cur.line-1];
    size_t prev_line_len_before_combining = prev_line->len;
    if (cur_line->len > 0) {
        // add the text from the current line to the prev line
        check_and_realloc_line(prev_line, cur_line->len);
        strcpy(prev_line->text + prev_line->len, cur_line->text);
        prev_line->len += cur_line->len;
    }

    // move subsequent lines up one
    Line **src = fp->lines + view->cur.line + 1;
    Line **dest = fp->lines + view->cur.line;
    memmove(dest, src, (fp->len - (view->cur.line + 1)) * sizeof(Line *));

    // update length
    fp->len -= 1;

    // update the line numbers of subsequent lines
    for (size_t i = view->cur.line; i < fp->len; i++) {
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

    move_up(*fp, view, ms);
    move_to_char(*fp, view, ms, prev_line_len_before_combining);
}

/**
 * Appends the text of the next line to the end of the current line.
 * Moves the cursor to where the newly inserted text begins on the current line.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new cursor position after combining
 */
void combine_line_with_next(FileProxy *fp, View *view, MimState ms) {
    if (view->cur.line == fp->len - 1) {
        return;
    }

    // move text
    Line *cur_line = fp->lines[view->cur.line];
    Line *next_line = fp->lines[view->cur.line+1];
    size_t cur_line_len_before_combining = cur_line->len;
    if (next_line->len > 0) {
        // add the text from the next line to the current line
        check_and_realloc_line(cur_line, next_line->len);
        strcpy(cur_line->text + cur_line->len, next_line->text);
        cur_line->len += next_line->len;
    }

    // move lines after next line up one
    if (view->cur.line < fp->len - 2) {
        Line **src = fp->lines + view->cur.line + 2;
        Line **dest = fp->lines + view->cur.line + 1;
        memmove(dest, src, (fp->len - (view->cur.line + 2)) * sizeof(Line *));
    }

    // update length
    fp->len -= 1;

    // update the line numbers of subsequent lines
    for (size_t i = view->cur.line + 1; i < fp->len; i++) {
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

    move_to_char(*fp, view, ms, cur_line_len_before_combining);
}

/**
 * Deletes the character before the cursor position. The cursor moves left one
 * to stay with the character it was on.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new view with the cursor moved one left
 */
void backspace(FileProxy *fp, View *view, MimState ms) {
    if (view->cur.ch == 0) {
        combine_line_with_prev(fp, view, ms);
        return;
    }

    Line *line = fp->lines[view->cur.line];
    check_and_realloc_line(line, -1);

    // move every char from cursor onwards left 1
    char *src = line->text + view->cur.ch;
    char *dest = src - 1;
    memmove(dest, src, (line->len - view->cur.ch + 1) * byte); // +1 for \0
    
    // update length
    line->len -= 1;
    
    // move cursor
    move_left(*fp, view);
}

void delete_char(FileProxy *fp, View *view, MimState ms) {
    Line *line = fp->lines[view->cur.line];
    if (view->cur.ch == line->len) {
        combine_line_with_next(fp, view, ms);
        return;
    }

    check_and_realloc_line(line, - 1);

    // move every char from char after cursor onwards left 1
    char *src = line->text + view->cur.ch + 1;
    char *dest = src - 1;
    // +1 for converting 0-indexed cur_ch to 1-indexed len, +1 for \0
    memmove(dest, src, (line->len - (view->cur.ch + 1) + 1) * byte); 
    
    // update length
    line->len -= 1;
}

/**
 * Inserts a new Line into the FileProxy after the current line.
 *
 * @param fp the FileProxy to edit
 * @param view the current View
 * @return the new view with the cursor at the beginning of the new line
 */
void insert_newline(FileProxy *fp, View *view, MimState ms) {
    // make space for a new line
    Line **tmp = realloc(fp->lines,  (fp->len + 1) * sizeof(Line *));
    if (tmp != NULL) {
        fp->lines = tmp;
    } else {
        fprintf(stderr, "Error reallocating space for new line.\n");
        exit(EXIT_FAILURE);
    }

    // if we aren't on the last line
    if (view->cur.line != fp->len - 1) {
        // move every line after the current down 1
        Line **src = fp->lines + view->cur.line + 1;
        Line **dest = fp->lines + view->cur.line + 2;
        memmove(dest, src, (fp->len - (view->cur.line + 1)) * sizeof(Line *));
    }

    // update length
    fp->len += 1;

    // insert new line
    fp->lines[view->cur.line+1] = create_line(view->cur.line + 1);

    // update the line numbers of each subsequent line
    for (size_t i = view->cur.line + 2; i < fp->len; i++) {
        fp->lines[i]->num += 1;
    }

    // move text
    Line *cur_line = fp->lines[view->cur.line];
    Line *new_line = fp->lines[view->cur.line+1];
    size_t text_to_eol_len = cur_line->len - view->cur.ch;

    // add the text from cursor to eol to the new line including indent
    size_t indent_len = get_len_ws_beginning(*cur_line);
    check_and_realloc_line(new_line, text_to_eol_len + indent_len);
    strncpy(new_line->text, cur_line->text, indent_len); // add indent
    strcpy(new_line->text + indent_len, cur_line->text + view->cur.ch); // add rest of text
    new_line->len += text_to_eol_len + indent_len;

    // remove the text from cursor to eol
    check_and_realloc_line(cur_line, -text_to_eol_len);
    cur_line->len -= text_to_eol_len;
    cur_line->text[cur_line->len] = '\0';

    // move to new line
    move_down(*fp, view, ms);
    move_to_bol_non_ws(*fp, view, ms);
}

