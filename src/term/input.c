#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

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
    erow * row = (editor.cy >= editor.numrows) ? NULL : &editor.row[editor.cy];

    switch (key) {

        case 'h': {
            if (editor.cx != 0) {
                editor.cx--;
            }
            break;
        }

        case 'l': {
            if (row && editor.cx < row->size) {
                editor.cx++;
            }
            break;
        }

        case 'k': {
            if (editor.cy != 0) {
                editor.cy--;
            }
            break;
        }

        case 'j': {
            if (editor.cy < editor.numrows - 1) {
                editor.cy++;
            }
            break;
            }
    }

    row = (editor.cy >= editor.numrows) ? NULL : &editor.row[editor.cy];
    int rowlen = row ? row->size : 0;
    if (editor.cx > rowlen) {
        editor.cx = rowlen;
    }
}

void handle_insert_keys() {
    int key = read_key();

    switch(key) {

        /* Prevent inserting escape sequences */
        case '\x1b':
        case '\033':
            break;

        /* Enter/Return key */
        case '\r':
            /* TODO */
            break;

        /* Backspace */
        case 127:
            /* TODO */
            break;

        case CTRL('n'): {
            editor.mode = 0;
        }

        case CTRL('h'): {
            move_cursor('h');
            break;
        }

        case CTRL('j'): {
            move_cursor('j');
            break;
        }

        case CTRL('k'): {
            move_cursor('k');
            break;
        }

        case CTRL('l'): {
            move_cursor('l');
            break;
        }

        default:
            insert_char(key);

    }

}

void handle_normal_keys() {
    int key = read_key();

    switch(key) {

        case 'q': {
            yy_quit();
            break;
        }

        case 'h': {
            move_cursor('h');
            break;
        }

        case 'j': {
            move_cursor('j');
            break;
        }

        case 'k': {
            move_cursor('k');
            break;
        }

        case 'l': {
            move_cursor('l');
            break;
        }

        case 'A': {
            if(editor.cy < editor.numrows) {
                editor.cx = editor.row[editor.cy].size;
                editor.mode = 1;
            }
            break;
        }

        case 'I': {
            editor.cx = 0;
            editor.mode = 1;
            break;
        }

        case 'i': {
            editor.mode = 1;
        }

    }

}
