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

#include "types.h"

Line *create_line(size_t line_num);

void check_and_realloc_line(Line *line, size_t additional_text_len);

FileProxy split_buffer(const char *buffer, size_t buf_len); 

void log_fp(FileProxy fp);

void free_fp(FileProxy fp);

void print_fp(FileProxy fp, View view);

void print_line(Line *line, View view);

#endif

