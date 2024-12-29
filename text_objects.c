/**
 * @file text_objects.c
 * @author Max Rimlinger
 *
 * Functions that define the beginnings and ends of text objects
 */

#include <ctype.h>
#include <stdbool.h>
#include <ctype.h>

#include "log.h"
#include "types.h"

bool is_word(const char ch) {
    return isalnum(ch) || ch == '_';
}

bool is_not_ws(const char ch) {
    return !isspace(ch);
}

bool is_not_word_not_ws(const char ch) {
    return !is_word(ch) && is_not_ws(ch);
}

CurPos get_beg_pos_cur_word(FileProxy fp, CurPos current_pos) {
    CurPos pos = current_pos;
    if (!is_word(fp.lines[current_pos.line]->text[current_pos.ch])) {
        for (size_t c = current_pos.ch; c-- > 0; ) {
            char ch = fp.lines[pos.line]->text[c];
            if (!is_word(ch)) {
                pos.ch = c;
            } else {
                return pos;
            }
        }
    } else {
        for (size_t c = current_pos.ch; c-- > 0; ) {
            char ch = fp.lines[current_pos.line]->text[c];
            if (is_word(ch)) {
                pos.ch = c;
            } else {
                return pos;
            }
        }
    }
    return pos;
}

CurPos get_end_pos_cur_word(FileProxy fp, CurPos current_pos) {
    CurPos pos = current_pos;
    if (!is_word(fp.lines[current_pos.line]->text[current_pos.ch])) {
        for (size_t c = current_pos.ch; c < fp.lines[current_pos.line]->len; c++) {
            char ch = fp.lines[pos.line]->text[c];
            if (!is_word(ch)) {
                pos.ch = c;
            } else {
                return pos;
            }
        }
    } else {
        for (size_t c = current_pos.ch; c < fp.lines[current_pos.line]->len; c++) {
            char ch = fp.lines[current_pos.line]->text[c];
            if (is_word(ch)) {
                pos.ch = c;
            } else {
                return pos;
            }
        }
    }
    return pos;
}

CurPos get_beg_pos_n_word(FileProxy fp, CurPos current_pos) {
    // FIXME ignores empty lines and doesn't like the last char of the line
    bool (*is_different)(const char);
    if (is_word(fp.lines[current_pos.line]->text[current_pos.ch])) {
        is_different = &is_not_word_not_ws;
    } else if (is_not_word_not_ws(fp.lines[current_pos.line]->text[current_pos.ch])) {
        is_different = &is_word;
    } else {
        is_different = &is_not_ws;
    }

    bool seen_ws = false;
    for (size_t l = current_pos.line; l < fp.len; l++) {
        size_t c;
        if (l == current_pos.line) {
            c = current_pos.ch;
        } else {
            // start at the first char on each line after the current
            c = 0;
            // also check that if we are on a new empty line because that counts
            // as the beginning of a word (but not the end aparrently)
            if (fp.lines[l]->len == 0) {
                CurPos new_pos = {l, c};
                return new_pos;
            }
            // a word of the same type (word or punctuation) continuing on the next line is a new word
            seen_ws = true;
        }
        for ( ; c < fp.lines[l]->len; c++) {
            char ch = fp.lines[l]->text[c];
            if (isspace(ch)) {
                seen_ws = true;
            }
            if (seen_ws) {
                // once we've seen whitespace, words or punctuation are fair game
                is_different = &is_not_ws;
            }
            if (is_different(ch)) {
                CurPos new_pos = {l, c};
                return new_pos;
            }
        }
    }
    return current_pos;
}

CurPos get_beg_pos_cur_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {
    switch (tobj) {
        case WORD:
            return get_beg_pos_cur_word(fp, current_pos);
    }
}

CurPos get_end_pos_cur_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {
    switch (tobj) {
        case WORD:
            return get_end_pos_cur_word(fp, current_pos);
    }
}

CurPos get_beg_pos_n_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {
    switch (tobj) {
        case WORD:
            return get_beg_pos_n_word(fp, current_pos);
    }
}

