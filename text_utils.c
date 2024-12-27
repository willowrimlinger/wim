/**
 * @file text_utils.c
 * @author Max Rimlinger
 *
 * Utilities to work with text in a FileProxy. Shared by motions.c and insert.c.
 */

#include <ctype.h>

#include "types.h"

size_t get_len_ws_beginning(Line line) {
    size_t ch_idx = 0;
    for (size_t i = 0; i < line.len; i++) {
        if (!isspace(line.text[i])) {
            break;
        }
        ch_idx = i + 1;
    }
    return ch_idx;
}
