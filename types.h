/**
 * @file types.h
 * @author Max Rimlinger
 *
 * Defines typedefed structs to be used as types throughout the program.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

/** Represents an individual line in a FileProxy */
typedef struct Line_s {
    char *text;
    size_t num;
    size_t len; // the number of characters in the line. doesn't count the \0.
    size_t cap;
} Line;

/** Represents a file and has some metadata information about line and buffer lengths */
typedef struct FileProxy_s {
    Line **lines;
    size_t len;
} FileProxy;

/** Represents a cursor position in a FileProxy as well as where in the file you are viewing */
typedef struct View_s {
    // the line that should be at the top of the screen
    size_t top_line;
    // the character/column that should be at the left of the screen
    size_t left_ch;
    // the number of lines available to display the FileProxy in
    size_t vlimit;
    // the number of cols available to display the FileProxy in
    size_t hlimit;
    // the line that the cursor is on
    size_t cur_line;
    // the character that the cursor is on
    size_t cur_ch;
    // the character that the cursor should be moved to if the line is long enough
    size_t cur_desired_ch;
} View;

/** The current mode of the mim program */
typedef enum Mode_e {
    NORMAL,
    INSERT
} Mode;

/**
 * Represents the internal state of the mim program. This stuff is "global" 
 * across buffers and there should only be one per program.
 */
typedef struct MimState_s {
    Mode mode;
} MimState;

#endif

