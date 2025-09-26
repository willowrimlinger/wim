/**
 * @file fileproxy.c
 * @author Willow Rimlinger
 *
 * Defines a proxy for a file and the lines contained within. This holds all 
 * the text including useful metadata such as how many lines in a file and how 
 * many characters in each line.
 */

#include <stdio.h>
#include <ncurses.h>

#include "log.h"
#include "types.h"
#include "fileproxy.h"
#include "motions.h"

static const size_t byte = sizeof(unsigned char);
static const size_t TEXT_BUF_INCR = 16;

/**
 * Creates a new empty Line with a beginning buffer capacity of TEXT_BUF_INCR
 *
 * @return the newly created line
 */
Line *create_line(size_t line_num) {
    Line *line = malloc(sizeof(Line));
    char *text = malloc(TEXT_BUF_INCR * byte + 1); // +1 for terminating null byte
    Line new_line = {text, line_num, 0, TEXT_BUF_INCR};
    *line = new_line;
    return line;
}

/**
 * Adjusts the text buffer of a line. Checks if the given line is full and the 
 * buffer needs to be increased or if the line has space to reduce the buffer.
 *
 * @param line the line to check and potentially increase
 */
void check_and_realloc_line(Line *line, size_t additional_text_len) {
    // check that we actually need to realloc
    size_t ideal_num_buffers = (line->len + additional_text_len + 1 / TEXT_BUF_INCR) + 1;
    size_t actual_num_buffers = (line->cap + 1 / TEXT_BUF_INCR);
//    log_to_file("line num:  %d", line->num);
//    log_to_file("ideal num buffers: %d", ideal_num_buffers);
//    log_to_file("actual num buffers: %d", actual_num_buffers);
    if (ideal_num_buffers != actual_num_buffers) {
        char *tmp = realloc(line->text, (ideal_num_buffers * TEXT_BUF_INCR) * byte + 1);
        if (tmp != NULL) {
            line->text = tmp;
        } else {
            fprintf(stderr, "Error reallocating space for line text.\n");
            exit(EXIT_FAILURE);
        }
        line->cap = ideal_num_buffers * TEXT_BUF_INCR;
    }
}

/**
 * Converts a text buffer containing the contents of a file into a FileProxy
 * 
 * @param buffer the text buffer to convert
 * @param buf_len the length in characters of the buffer
 * @return A FileProxy whose Lines have been filled with the contents of the buffer
 */
FileProxy split_buffer(const char *buffer, size_t buf_len) {
    size_t num_lines = 1;
    for (size_t i = 0; i < buf_len; i++) {
        if (buffer[i] == '\n' && i != buf_len - 1) {
            num_lines++;
        }
    }
    Line **lines = malloc(sizeof(Line *) * num_lines);
    size_t lines_idx = 0;

    Line *first_line = create_line(lines_idx);
    lines[lines_idx] = first_line;

    for (size_t i = 0; i < buf_len - 1; i++) {
        // create a new line
        if (buffer[i] == '\n') {
            lines_idx++;
            Line *line = create_line(lines_idx);
            lines[lines_idx] = line;
        } else {
            // increase the text buffer size of the line if it is full
            Line *line = lines[lines_idx];
            check_and_realloc_line(line, 1);

            // add a character on to the line
            line->text[line->len] = buffer[i];
            line->text[line->len+1] = '\0';
            line->len = line->len + 1;
        }
    }
    FileProxy fp = {lines, num_lines};
    log_fp(fp);
    return fp;
}

/** Debug function to see everything about a FileProxy */
void log_fp(FileProxy fp) {
    log_to_file("fileproxy:");
    for (size_t i = 0; i < fp.len; i++) {
        log_to_file("line num: %lu", fp.lines[i]->num);
        log_to_file("len: %lu", fp.lines[i]->len);
        log_to_file("cap: %lu", fp.lines[i]->cap);
        log_to_file("text: %s", fp.lines[i]->text);
    }
}

/**
 * Frees a FileProxy and all text within it
 *
 * @param fp the FileProxy to free
 */
void free_fp(FileProxy fp) {
    for (size_t i = 0; i < fp.len; i++) {
        free(fp.lines[i]->text);
        fp.lines[i]->text = NULL;
        free(fp.lines[i]);
        fp.lines[i] = NULL;
    }
    free(fp.lines);
    fp.lines = NULL;
}

/** 
 * Prints the contents of a FileProxy to the ncurses stdscr
 * TODO breaks if file extends horizontally or vertically past the viewport
 * need to implement either text wrapping, text overflow, or both
 *
 * @param fp the FileProxy to print
 */
void print_fp(FileProxy fp, View view) {
    erase();
    for (size_t i = view.top_line; i < fp.len; i++) {
        Line line = *fp.lines[i];
        for (size_t j = view.left_ch; j < line.len; j++) {
            mvaddch(i - view.top_line, j - view.left_ch, line.text[j]);
        }
    }
}

