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

void move_cursor(char key) {
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

void handle_normal_keys() {
    int key = read_key();

    switch(key) {
        case 'q':
            yy_quit();
            break;
        case 'h':
        case 'j':
        case 'k':
        case 'l':
            move_cursor(key);
        case 'i':
            editor.mode = 1;
        default:
            insert_char(key);
    }
}

void handle_insert_keys() {
    int key = read_key();

    switch(key) {
        case CTRL('h'):
        case CTRL('j'):
        case CTRL('k'):
        case CTRL('l'):
            move_cursor(key);
        case CTRL('q'):
            editor.mode = 1;
        default:
            insert_char(key);
    }

}
