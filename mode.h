/**
 * @file mode.h
 * @author Max Rimlinger
 *
 * Functions for changing modes that will handle the side effects of doing so
 */

#ifndef MODE_H
#define MODE_H

#include "types.h"

void switch_mode(FileProxy fp, View *view, MimState *ms, Mode new_mode);

#endif
