/**
 * @file log.h
 * @author Max Rimlinger
 *
 * Functions to log info to a log file
 */

#include "types.h"

void log_to_file(const char *fmt, ...);

void clear_log();

void log_to_status_bar(MimState *ms, const char *fmt, ...);
