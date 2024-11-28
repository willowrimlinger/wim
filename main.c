/**
 * @file main.c
 * @author Max Rimlinger
 *
 * mim - a stupid text editor
 */

#include <stdio.h>
#include <string.h>
#include <ncurses.h>

/** The size of the buffer for reading files. */
const size_t BUFSIZE = 256;

int main(int argc, char *argv[]) {
    initscr();

    // open a file based on cmd line args
    FILE *file = fopen(argv[1], "r");

    char buffer[BUFSIZE];
    size_t byte = sizeof(unsigned char);
    size_t num_elements;
    while ((num_elements = fread(buffer, byte, BUFSIZE / byte, file)) != 0) {
        // prevent a partially full buffer from being read past the EOF
        buffer[num_elements] = '\0';
        for (int i = 0; i < num_elements; i++) {
            char silly_string[2];
            silly_string[0] = buffer[i];
            silly_string[1] = '\0';
            printw(silly_string);
        }
    }

    fclose(file);

    refresh();
    getch();

    endwin();
    return 0;
}
