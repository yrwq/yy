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

void move_cursor_normal(char key) {
    switch (key) {
        case 'h':
            if (editor.cx != 0) {
                editor.cx--;
            }
            break;
        case 'l':
            if (editor.cx != editor.cols - 1) {
                editor.cx++;
            }
            break;
        case 'k':
            if (editor.cy != 0) {
                editor.cy--;
            }
            break;
        case 'j':
            if (editor.cy != editor.rows - 1) {
                editor.cy++;
            }
            break;
    }
}

/* Handle each key */
void handle_keys() {
    int key = read_key();

    switch(key) {
        case CTRL('q'):
            yy_quit();
            break;
        case 'h':
        case 'j':
        case 'k':
        case 'l':
            move_cursor_normal(key);
        default:
            insert_char(key);
    }

}
