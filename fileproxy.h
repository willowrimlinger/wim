/**
 * @file fileproxy.h
 * @author Max Rimlinger
 * 
 * Header for fileproxy.c
 *
 * Defines a proxy for a file and the lines contained within. This holds all 
 * the text including useful metadata such as how many lines in a file and how 
 * many characters in each line.
 */

#ifndef FILEPROXY_H
#define FILEPROXY_H

#include <stdlib.h>
#include <stdbool.h>

#include "types.h"

/**
 * Creates a new empty Line with a beginning buffer capacity of TEXT_BUF_INCR
 *
 * @return the newly created line
 */
Line *create_line(size_t line_num);

/**
 * Adjusts the text buffer of a line. Checks if the given line is full and the 
 * buffer needs to be increased or if the line has space to reduce the buffer.
 * Adjusts the capacity of the line.
 *
 * @param line the line to check and potentially increase
 * @param additional_text_len the number of chars that are being added (or subtracted), not including \0
 *     from the line from which the new buffer calculations will be adjusted to
 */
void check_and_realloc_line(Line *line, size_t additional_text_len);

/**
 * Checks if the text in a Line equals a string.
 *
 * @param line the line to check the text of
 * @param string the string to compare against the line text
 * @return true if the text matches, false otherwise
 */
bool linecmp(Line *line, const char *string);

/**
 * Creates an empty FileProxy with a single blank line.
 *
 * @return an empty FileProxy
 */
FileProxy create_empty_fp();

/**
 * Converts a text buffer containing the contents of a file into a FileProxy
 * 
 * @param buffer the text buffer to convert
 * @param buf_len the length in characters of the buffer
 * @return A FileProxy whose Lines have been filled with the contents of the buffer
 */
FileProxy split_buffer(const char *buffer, size_t buf_len); 

/** Debug function to see everything about a FileProxy */
void log_fp(FileProxy fp);

/**
 * Frees a FileProxy and all text within it
 *
 * @param fp the FileProxy to free
 */
void free_fp(FileProxy fp);

/**
 * Writes the contents of a FileProxy to a file, overwriting all previous contents
 *
 * @param fp the FileProxy to write to disk
 * @param filename the name of the file to write to
 */
void write_fp(FileProxy fp, const char *filename);

#endif

