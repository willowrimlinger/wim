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
    size_t len;
    size_t cap;
} Line;

typedef struct FileProxy_s {
    Line **lines;
    size_t len;
} FileProxy;

FileProxy split_buffer(char *buffer, size_t buf_len); 

void free_fp(FileProxy fp);

void print_fp(FileProxy fp);

#endif

