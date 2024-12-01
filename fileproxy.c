/**
 * @file fileproxy.c
 * @author Max Rimlinger
 *
 * Defines a proxy for a file and the lines contained within. This holds all 
 * the text including useful metadata such as how many lines in a file and how 
 * many characters in each line.
 */

#include <stdio.h>
#include <ncurses.h>

#include "fileproxy.h"

const size_t TEXT_BUF_INCR = 16;

FileProxy split_buffer(char *buffer, size_t buf_len) {
    size_t num_lines = 1;
    for (size_t i = 0; i < buf_len; i++) {
        if (buffer[i] == '\n' && i != buf_len - 1) {
            num_lines++;
        }
    }
    Line **lines = malloc(sizeof(Line *) * num_lines);
    size_t lines_idx = 0;

    // create the first line
    Line *line = malloc(sizeof(Line));
    size_t byte = sizeof(unsigned char);
    char *text = malloc(TEXT_BUF_INCR * byte + 1); // +1 for terminating null byte
    Line new_line = {text, 0, TEXT_BUF_INCR};
    *line = new_line;
    lines[lines_idx] = line;

    for (size_t i = 0; i < buf_len - 1; i++) {
        // create a new line
        if (buffer[i] == '\n') {
            Line *line = malloc(sizeof(Line));
            char *text = malloc(TEXT_BUF_INCR * byte + 1); // +1 for terminating null byte
            Line new_line = {text, 0, TEXT_BUF_INCR};
            *line = new_line;
            lines_idx++;
            lines[lines_idx] = line;
        } else {
            // increase the text buffer size of the line if it is full
            size_t text_len = lines[lines_idx]->len;
            size_t cap = lines[lines_idx]->cap;
            size_t byte = sizeof(unsigned char);
            if (text_len == cap) {
                char *tmp = realloc(lines[lines_idx]->text, text_len + (TEXT_BUF_INCR * byte) + 1);
                if (tmp != NULL) {
                    lines[lines_idx]->text = tmp;
                } else {
                    perror("Error reallocating space for line text");
                    exit(EXIT_FAILURE);
                }
                lines[lines_idx]->cap = text_len + TEXT_BUF_INCR;
            }

            // add a character on to the line
            lines[lines_idx]->text[text_len] = buffer[i];
            lines[lines_idx]->text[text_len+1] = '\0';
            lines[lines_idx]->len = text_len + 1;
        }
    }
    FileProxy fp = {lines, num_lines};
    return fp;
}

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

void print_fp(FileProxy fp) {
    // TODO breaks if file extends horizontally or vertically past the viewport
    // need to implement either text wrapping, text overflow, or both
    for (size_t i = 0; i < fp.len; i++) {
        Line line = *fp.lines[i];
        for (size_t j = 0; j < line.len; j++) {
            mvaddch(i, j, line.text[j]);
        }
    }
}

