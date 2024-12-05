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

Line *create_line(size_t line_num);

void check_and_realloc_line(Line *line);

FileProxy split_buffer(const char *buffer, size_t buf_len); 

void free_fp(FileProxy fp);

void print_fp(FileProxy fp);

#endif

