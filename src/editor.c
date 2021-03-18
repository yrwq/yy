#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <term/input.h>
#include <term/mode.h>
#include <editor.h>

editor_state_t editor = {};

/* Draw a '~' in the beginning of each line */
void draw_rows(struct abuf * ab) {
    int y;
    for (y = 0; y < editor.rows; y++) {
        buf_append(ab, "~", 1);
        buf_append(ab, "\033[K", 3);
        if (y < editor.rows -1) {
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

/* Get the cursor's position */
int get_cursor_position(int * rows, int * cols) {
    char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }

    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    return 0;
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

void yy_init() {
    term_setup();
    editor.cx = 0;
    editor.cy = 0;
    get_term_size(&editor.rows, &editor.cols);
    editor.running = 1;
}

void yy_refresh() {

    struct abuf ab = ABUF_INIT;

    buf_append(&ab, "\x1b[?25l", 6);
    buf_append(&ab, "\x1b[H", 3);

    draw_rows(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", editor.cy + 1, editor.cx + 1);
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
    /* Close file */
    exit(1);
}

