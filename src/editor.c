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

/* Holds the state of the editor, */
/* See 'editor.h' for options */
editor_state_t editor = {};

/* Draw file line by line.
 * After the end of the file,
 * draw a '~' in the beginning of each line */
void draw_rows(struct abuf *ab) {
    int y;
    for (y = 0; y < editor.rows; y++) {
        int filerow = y + editor.rowoff;
        if (filerow >= editor.numrows) {
                buf_append(ab, "~", 1);
        } else {
            int len = editor.row[filerow].rsize - editor.coloff;
            if (len < 0) len = 0;
            if (len > editor.cols) len = editor.cols;
            buf_append(ab, &editor.row[filerow].render[editor.coloff], len);
        }
        buf_append(ab, "\x1b[K", 3);
        if (y < editor.rows - 1) {
            buf_append(ab, "\r\n", 2);
        }
    }
}

/* Insert a character to the buffer */
void insert_char(char c) {
    /* TODO */
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

/* Append to the buffer */
void buf_append(struct abuf * ab, const char * s, int len) {
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL) return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

/* Free buffer's memory */
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

/* Initialize the editor */
void yy_init() {
    editor.running = 1;
    /* Initialize termios, see term/mode.c */
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

/* Refresh the editor */
void yy_refresh() {
    /* Proper scrolling */
    scrolloff();
    /* Initialize an empty buffer */
    struct abuf ab = ABUF_INIT;

    /* Handle different input modes, like in vim */
    if (editor.mode == 1){
        handle_insert_keys();
    } else {
        handle_normal_keys();
    }

    /* Set the cursor to 0,0 */
    buf_append(&ab, "\x1b[H", 3);

    draw_rows(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (editor.cy - editor.rowoff) + 1,
                                              (editor.cx - editor.coloff) + 1);
    buf_append(&ab, buf, strlen(buf));

    write(STDOUT_FILENO, ab.b, ab.len);

    buf_free(&ab);

}

/* Quit the editor */
void yy_quit() {
    /* Clear the whole terminal */
    write(STDOUT_FILENO, "\x1b[2J", 4);
    /* Set the cursor to 0,0 */
    write(STDOUT_FILENO, "\x1b[H", 3);
    /* Reset the state we saved, when initializing termios */
    term_reset();
    editor.running = 0;
    exit(1);
}

/* Load a file */
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

/* Fill the contents of 'render' with 'chars' string from 'erow' */
/* Copy each character from 'chars' to 'render' */
void update_row(erow *row) {
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t') tabs++;

    free(row->render);
    row->render = malloc(row->size + tabs*(TAB_STOP - 1) + 1);

    int idx = 0;
    for (j = 0; j < row->size; j++) {
        if (row->chars[j] == '\t') {
            row->render[idx++] = ' ';
            while (idx % TAB_STOP != 0) row->render[idx++] = ' ';
        } else {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}


/* Append a row */
void append_row(char *s, size_t len) {
    editor.row = realloc(editor.row, sizeof(erow) * (editor.numrows + 1));

    int at = editor.numrows;
    editor.row[at].size = len;
    editor.row[at].chars = malloc(len + 1);
    memcpy(editor.row[at].chars, s, len);
    editor.row[at].chars[len] = '\0';

    editor.row[at].rsize = 0;
    editor.row[at].render = NULL;
    update_row(&editor.row[at]);

    editor.numrows++;
}
