#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <term/mode.h>

#define CTRL_KEY(k) ((k) & 0x1f)

/* Read a keypress as a character */
char read_key() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            exit(0);
    }

    return c;
}

/* Handle each key */
void handle_keys() {
    char c = read_key();

    switch (c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
        default:
            printf("%c", c);
            fflush(stdout);
    }
}

/* Set the cursor to x,y */
void set_cursor(int x, int y) {
    printf("\033[%u;%uH", x + 1, y + 1);
    fflush(stdout);
}

/* Clear the terminal */
void term_clear() {
    printf("\033[2J");
    fflush(stdout);
}
