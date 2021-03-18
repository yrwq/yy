#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <term/input.h>
#include <term/mode.h>
#include <editor.h>

editor_state_t editor = {};

void draw_rows(struct abuf *ab) {
    int y;
    for (y = 0; y < editor.rows; y++) {
        int filerow = y + editor.rowoff;
        if (filerow >= editor.numrows) {
            if (editor.numrows == 0 && y == editor.rows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "YY version 0.1");
                if (welcomelen > editor.cols) welcomelen = editor.cols;
                int padding = (editor.cols - welcomelen) / 2;
                if (padding) {
                    buf_append(ab, "~", 1);
                    padding--;
                }
                while (padding--) buf_append(ab, " ", 1);
                buf_append(ab, welcome, welcomelen);
            } else {
                buf_append(ab, "~", 1);
            }
        } else {
            int len = editor.row[filerow].size - editor.coloff;
            if (len < 0) len = 0;
            if (len > editor.cols) len = editor.cols;
            buf_append(ab, &editor.row[filerow].chars[editor.coloff], len);

            /* int len = editor.row[filerow].size; */
            /* if (len > editor.cols) len = editor.cols; */
            /* buf_append(ab, editor.row[filerow].chars, len); */
        }

        buf_append(ab, "\x1b[K", 3);
        if (y < editor.rows - 1) {
            buf_append(ab, "\r\n", 2);
        }
    }
}

/* Insert a character */
void insert_char(char c) {
    printf("%c", c);
    fflush(stdout);
}

/* Get the terminal's size */
int get_term_size(int * rows, int * cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void buf_append(struct abuf * ab, const char * s, int len) {
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL) return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void buf_free(struct abuf * ab) {
    free(ab->b);
}

/* Check if the cursor has moved outside of the visible */
/* window, if so adjust the rows */
void scrolloff() {
    if (editor.cy < editor.rowoff) {
        editor.rowoff = editor.cy;
    }
    if (editor.cy >= editor.rowoff + editor.rows) {
        editor.rowoff = editor.cy - editor.rows + 1;
    }
    if (editor.cx < editor.coloff) {
        editor.coloff = editor.cx;
    }
    if (editor.cx >= editor.coloff + editor.cols) {
        editor.coloff = editor.cx - editor.cols + 1;
    }
}

/* Initialize the editor with optional settings */
void yy_init() {
    editor.running = 1;
    term_setup();
    get_term_size(&editor.rows, &editor.cols);

    editor.cx = 0;
    editor.cy = 0;
    editor.numrows = 0;
    editor.rowoff = 0;
    editor.coloff = 0;
    editor.row = NULL;
    editor.mode = 0;
}

void yy_refresh() {
    scrolloff();
    struct abuf ab = ABUF_INIT;

    buf_append(&ab, "\x1b[?25l", 6);
    buf_append(&ab, "\x1b[H", 3);

    draw_rows(&ab);

    char buf[32];
    /* snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (editor.cy - editor.rowoff) + 1, editor.cx + 1); */

    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (editor.cy - editor.rowoff) + 1,
                                              (editor.cx - editor.coloff) + 1);
    buf_append(&ab, buf, strlen(buf));

    buf_append(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);

    buf_free(&ab);

}

void yy_quit() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    term_reset();
    editor.running = 0;
    exit(1);
}

void load_file(char * filename) {
    FILE * fp = fopen(filename, "r");
    char * line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                            line[linelen - 1] == '\r'))
        linelen--;
        append_row(line, linelen);
    }
    free(line);
    fclose(fp);
}


void append_row(char *s, size_t len) {
    editor.row = realloc(editor.row, sizeof(erow) * (editor.numrows + 1));

    int at = editor.numrows;
    editor.row[at].size = len;
    editor.row[at].chars = malloc(len + 1);
    memcpy(editor.row[at].chars, s, len);
    editor.row[at].chars[len] = '\0';
    editor.numrows++;
}
