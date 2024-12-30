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

bool is_not_word(const char ch) {
    return !is_word(ch);
}

bool is_word_or_ws(const char ch) {
    return is_word(ch) || isspace(ch);
}

bool is_not_ws(const char ch) {
    return !isspace(ch);
}

bool is_not_word_not_ws(const char ch) {
    return !is_word(ch) && is_not_ws(ch);
}

CurPos get_beg_pos_cur_word(FileProxy fp, CurPos current_pos) {
    bool (*is_different)(const char);
    if (is_word(fp.lines[current_pos.line]->text[current_pos.ch])) {
        is_different = &is_not_word;
    } else if (is_not_word_not_ws(fp.lines[current_pos.line]->text[current_pos.ch])) {
        is_different = &is_word_or_ws;
    } else {
        is_different = &is_not_ws;
    }

    CurPos pos = current_pos;
    for (size_t c = current_pos.ch; c-- > 0; ) {
        char ch = fp.lines[pos.line]->text[c];
        if (is_different(ch)) {
            log_to_file("the beginning of the word is at l: %lu, c: %lu", pos.line, pos.ch);
            return pos;
        }
        pos.ch = c;
    }
    return pos;
}

CurPos get_beg_pos_n_word(FileProxy fp, CurPos current_pos) {
    bool (*is_different)(const char);
    if (is_word(fp.lines[current_pos.line]->text[current_pos.ch])) {
        is_different = &is_not_word_not_ws;
    } else if (is_not_word_not_ws(fp.lines[current_pos.line]->text[current_pos.ch])) {
        is_different = &is_word;
    } else {
        is_different = &is_not_ws;
    }

    CurPos pos = current_pos;
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
        for (/* see above */ ; c < fp.lines[l]->len; c++) {
            char ch = fp.lines[l]->text[c];
            CurPos new_pos = {l, c};
            pos = new_pos;
            if (isspace(ch)) {
                seen_ws = true;
            }
            if (seen_ws) {
                // once we've seen whitespace, words or punctuation are fair game
                is_different = &is_not_ws;
            }
            if (is_different(ch)) {
                // we found it
                return pos;
            }
        }
    }
    // this results in us going to the end of the buffer if there aren't any more words
    return pos;
}

CurPos get_beg_pos_p_word(FileProxy fp, CurPos current_pos) {
    char beg_ch = fp.lines[current_pos.line]->text[current_pos.ch];
    if (is_not_ws(beg_ch)) {
        CurPos beg_pos_cur_word = get_beg_pos_cur_word(fp, current_pos);
        if (
            beg_pos_cur_word.line != current_pos.line
            || beg_pos_cur_word.ch != current_pos.ch
        ) {
            return beg_pos_cur_word;
        }
    }

    bool (*is_different)(const char);
    if (is_word(beg_ch)) {
        is_different = &is_not_word_not_ws;
    } else if (is_not_word_not_ws(beg_ch)) {
        is_different = &is_word;
    } else {
        is_different = &is_not_ws;
    }

    CurPos pos = current_pos;
    bool seen_ws = false;
    for (size_t l = current_pos.line + 1; l-- > 0; ) {
        size_t c;
        if (l == current_pos.line) {
            c = current_pos.ch + 1;
        } else {
            // start at the last char of each line after the current
            c = fp.lines[l]->len;

            // also check that if we are on a new empty line because that counts
            // as the beginning of a word (but not the end aparrently)
            if (fp.lines[l]->len == 0) {
                CurPos new_pos = {l, c};
                return new_pos;
            }
            // a word of the same type (word or punctuation) continuing on the next line is a new word
            seen_ws = true;
        }
        for (/* see above */ ; c-- > 0; ) {
            char ch = fp.lines[l]->text[c];
            CurPos new_pos = {l, c};
            pos = new_pos;
            if (isspace(ch)) {
                seen_ws = true;
            }
            if (seen_ws) {
                // once we've seen whitespace, words or punctuation are fair game
                is_different = &is_not_ws;
            }
            if (is_different(ch)) {
                // we found it
                return get_beg_pos_cur_word(fp, pos);
            }
        }
    }
    // this results in us going to the end of the buffer if there aren't any more words
    return get_beg_pos_cur_word(fp, pos);
}

CurPos get_beg_pos_cur_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {
    switch (tobj) {
        case WORD:
            return get_beg_pos_cur_word(fp, current_pos);
    }
}

/*CurPos get_end_pos_cur_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {*/
/*    switch (tobj) {*/
/*        case WORD:*/
/*            return get_end_pos_cur_word(fp, current_pos);*/
/*    }*/
/*}*/
/**/
CurPos get_beg_pos_n_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {
    switch (tobj) {
        case WORD:
            return get_beg_pos_n_word(fp, current_pos);
    }
}

CurPos get_beg_pos_p_tobj(FileProxy fp, CurPos current_pos, TextObject tobj) {
    switch (tobj) {
        case WORD:
            return get_beg_pos_p_word(fp, current_pos);
    }
}

