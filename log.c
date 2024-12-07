/**
 * @file log.c
 * @author Max Rimlinger
 *
 * Functions to log info to a log file
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

/**
 * Logs a message to the mim.log file. No \n needed at the end of the message.
 *
 * @param msg the message to log
 */
void log_to_file(const char *msg) {
    FILE *file = fopen("mim.log", "a");
    if (file == NULL) {
        fprintf(stderr, "File mim.log not found.\n");
    }

    time_t curr_time = time(NULL);
    char *time_str = ctime(&curr_time);
    time_str[strlen(time_str)-1] = '\0';

    fprintf(file, "[%s] %s\n", time_str, msg);

    fclose(file);
}
