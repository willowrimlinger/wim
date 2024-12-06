/**
 * @file types.h
 * @author Max Rimlinger
 *
 * Defines typedefed structs to be used as types throughout the program.
 */

#ifndef TYPES_H
#define TYPES_H

typedef struct Line_s {
    char *text;
    size_t num;
    size_t len; // the number of characters in the line. doesn't count the \0.
    size_t cap;
} Line;

typedef struct FileProxy_s {
    Line **lines;
    size_t len;
} FileProxy;

/** Represents a cursor position in a file as well as where in the file you are viewing */
typedef struct View_s {
    // the line that should be at the top of the screen
    size_t top_line;
    // the character/column that should be at the left of the screen
    size_t left_ch;
    // the line that the cursor is on
    size_t cur_line;
    // the character that the cursor is on
    size_t cur_ch;
    // the character that the cursor should be moved to if the line is long enough
    size_t cur_desired_ch;
} View;

#endif

