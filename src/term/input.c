#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <term/mode.h>
#include <editor.h>

/* Read a key */
int read_key() {
    char c = 0;

    if(read(STDIN_FILENO, &c, 1) == -1) {
        exit(1);
    }

    return c;
}

/* Handle each key */
void handle_keys() {
    int key = read_key();

    switch(key) {
        case CTRL('q'):
            yy_quit();
            break;
        default:
            insert_char(key);
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
