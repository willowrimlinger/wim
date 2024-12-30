/**
 * @file text_objects.c
 * @author Max Rimlinger
 *
 * Functions that define the beginnings and ends of text objects
 */

#include "types.h"

/**
 * Get the position in a FileProxy of the first character of a text object
 *
 * @param fp the fileproxy to find the text object in
 * @param current_pos the current cursor position that is within the text object
 * @param tobj the text object to find the first character of
 * @return the position in the fileproxy of the first character of the text object
 */
CurPos get_beg_pos_tobj(FileProxy fp, CurPos current_pos, TextObject tobj);

/**
 * Get the position in a FileProxy of the first character of a text object
 *
 * @param fp the fileproxy to find the text object in
 * @param current_pos the current cursor position that is within the text object
 * @param tobj the text object to find the first character of
 * @return the position in the fileproxy of the first character of the text object
 */
CurPos get_end_pos_tobj(FileProxy fp, CurPos current_pos, TextObject tobj);

/**
 * Get the position in a FileProxy of the first character of the next text object
 * that the current_pos is not already inside.
 *
 * @param fp the fileproxy to find the text object in
 * @param current_pos the current cursor position
 * @param tobj the text object to find the first character of
 * @return the position in the fileproxy of the first character of the next text object
 */
CurPos get_beg_pos_n_tobj(FileProxy fp, CurPos current_pos, TextObject tobj);

/**
 * Get the position in a FileProxy of the first character of the previous text object
 * that the current_pos is not already inside.
 *
 * @param fp the fileproxy to find the text object in
 * @param current_pos the current cursor position
 * @param tobj the text object to find the first character of
 * @return the position in the fileproxy of the first character of the previous text object
 */
CurPos get_beg_pos_p_tobj(FileProxy fp, CurPos current_pos, TextObject tobj);
