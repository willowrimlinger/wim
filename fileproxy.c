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

#include "fileproxy.h"

const size_t TEXT_BUF_INCR = 16;

/**
 * Creates a new empty Line with a beginning buffer capacity of TEXT_BUF_INCR
 *
 * @return the newly created line
 */
Line *create_line() {
    Line *line = malloc(sizeof(Line));
    size_t byte = sizeof(unsigned char);
    char *text = malloc(TEXT_BUF_INCR * byte + 1); // +1 for terminating null byte
    Line new_line = {text, 0, TEXT_BUF_INCR};
    *line = new_line;
    return line;
}

/**
 * Checks if a give line is full and if so, increases the size of the text buffer.
 *
 * @param line the line to check and potentially increase
 */
void check_and_realloc_line(Line *line) {
    size_t byte = sizeof(unsigned char);
    if (line->len == line->cap) {
        char *tmp = realloc(line->text, line->len + (TEXT_BUF_INCR * byte) + 1);
        if (tmp != NULL) {
            line->text = tmp;
        } else {
            perror("Error reallocating space for line text");
            exit(EXIT_FAILURE);
        }
        line->cap = line->len + TEXT_BUF_INCR;
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

    Line *first_line = create_line();
    lines[lines_idx] = first_line;

    for (size_t i = 0; i < buf_len - 1; i++) {
        // create a new line
        if (buffer[i] == '\n') {
            Line *line = create_line();
            lines_idx++;
            lines[lines_idx] = line;
        } else {
            // increase the text buffer size of the line if it is full
            Line *line = lines[lines_idx];
            check_and_realloc_line(lines[lines_idx]);

            // add a character on to the line
            line->text[line->len] = buffer[i];
            line->text[line->len+1] = '\0';
            line->len = line->len + 1;
        }
    }
    FileProxy fp = {lines, num_lines};
    return fp;
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
 * Prints the contents of a FileProxy to the ncurses stdscr.
 * TODO breaks if file extends horizontally or vertically past the viewport
 * need to implement either text wrapping, text overflow, or both
 *
 * @param fp the FileProxy to print
 */
void print_fp(FileProxy fp) {
    for (size_t i = 0; i < fp.len; i++) {
        Line line = *fp.lines[i];
        for (size_t j = 0; j < line.len; j++) {
            mvaddch(i, j, line.text[j]);
        }
    }
}

