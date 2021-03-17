#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <term/mode.h>
#include <editor.h>

/* Read a key */
int read_key(size_t *modifiers_out) {
    char c = 0;
    if(read(STDIN_FILENO, &c, 1) == -1) {
        exit(1);
    }

    if(c == 0x1b) {
        size_t modifiers = 0;
        char buffer[5];
        if(read(STDIN_FILENO, buffer, 5) == -1) {
            exit(1);
        }

        if(buffer[0] == '[') {
            size_t i = 1;
            if(buffer[1] == '1' && buffer[2] == ';') {
                if(buffer[3] == '2')
                    modifiers |= MODIFIER_SHIFT;
                else if(buffer[3] == '5')
                    modifiers |= MODIFIER_CTRL;
                i += 3;
            }

            if(modifiers_out != NULL)
                *modifiers_out = modifiers;
            if(buffer[i] == 'A')
                return KEY_ARROW_UP;
            else if(buffer[i] == 'B')
                return KEY_ARROW_DOWN;
            else if(buffer[i] == 'C')
                return KEY_ARROW_RIGHT;
            else if(buffer[i] == 'D')
                return KEY_ARROW_LEFT;
        }
        return 0;
    }

    return c;
}

/* Handle each key */
void handle_keys() {
    size_t modifiers;
    int key = read_key(&modifiers);

    if(key == 0)
        return;

    switch(key) {
    case CTRL('q'):
        quit();
        break;

    case KEY_ARROW_UP:
    case CTRL('k'):
        move_cursor(0, -1, 1, 1);
        break;

    case KEY_ARROW_DOWN:
    case CTRL('j'):
        move_cursor(0, 1, 1, 1);
        break;

    case KEY_ARROW_LEFT:
    case CTRL('h'):
        move_cursor(-1, 0, 1, 1);
        break;

    case KEY_ARROW_RIGHT:
    case CTRL('l'):
        move_cursor(1, 0, 1, 1);
        break;

    case CTRL('d'): {
        size_t cursor_x_limit = strlen(editor.buffer.lines[editor.buffer.cursor_y]);
        move_cursor(cursor_x_limit, editor.buffer.cursor_y, 1, 0);
    } break;

    case CTRL('a'):
        move_cursor(0, editor.buffer.cursor_y, 1, 0);
        break;

    case KEY_BACKSPACE:
        delete_char();
        break;

    case KEY_RETURN:
        insert_line();
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
