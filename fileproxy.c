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
    char *text = malloc((TEXT_BUF_INCR + 1) * byte); // +1 for terminating null byte
    Line new_line = {text, line_num, 0, TEXT_BUF_INCR};
    *line = new_line;
    return line;
}

/**
 * Adjusts the text buffer of a line. Checks if the given line is full and the 
 * buffer needs to be increased or if the line has space to reduce the buffer.
 * Adjusts the capacity of the line.
 *
 * @param line the line to check and potentially increase
 * @param additional_text_len the number of chars that are being added (or subtracted), not including \0
 *     from the line from which the new buffer calculations will be adjusted to
 */
void check_and_realloc_line(Line *line, size_t additional_text_len) {
    // check that we actually need to realloc
    size_t ideal_num_buffers = ((line->len + additional_text_len + 1) / TEXT_BUF_INCR) + 1;
    size_t actual_num_buffers = ((line->cap + 1) / TEXT_BUF_INCR);
    if (ideal_num_buffers != actual_num_buffers) {
        char *tmp = realloc(line->text, (ideal_num_buffers * TEXT_BUF_INCR + 1) * byte);
        if (tmp != NULL) {
            line->text = tmp;
        } else {
            fprintf(stderr, "Error reallocating space for line text.\n");
            exit(EXIT_FAILURE);
        }
//        line->len += additional_text_len;
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
    return fp;
}

/** Debug function to see everything about a FileProxy */
void log_fp(FileProxy fp) {
    log_to_file("fileproxy:");
    for (size_t i = 0; i < fp.len; i++) {
        log_to_file("line num: %lu", fp.lines[i]->num);
        log_to_file("len: %lu", fp.lines[i]->len);
        log_to_file("cap: %lu", fp.lines[i]->cap);
        if (fp.lines[i]->len == 0) {
            log_to_file("text: <empty>");
        } else {
            log_to_file("text: %s", fp.lines[i]->text);
        }
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

// FIXME acts weird sometimes. duplicates lines or removes them
void write_fp(FileProxy fp, const char *filename) {
    log_fp(fp);
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error saving %s\n", filename);
    }

    for (size_t i = 0; i < fp.len; i++) {
        if (fp.lines[i]->len > 0) {
            fprintf(file, "%s", fp.lines[i]->text);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

