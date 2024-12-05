/**
 * @file insert.h
 * @author Max Rimlinger
 *
 * Conatins functions for inserting and deleting text from the file
 */

#include "fileproxy.h"
#include "motions.h"

#ifndef INSERT_H
#define INSERT_H

Pos insert_char(char ch, FileProxy fp, Pos pos);

#endif
