#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <term/input.h>
#include <term/mode.h>
#include <editor.h>

editor_state_t editor = {};

void draw_rows() {
    int y;
    for (y = 0; y < editor.rows; y++) {
        printf("~\r\n");
    }
    set_cursor(0, 0);
}


void insert_char(char c) {
    printf("%c", c);
    fflush(stdout);
}

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

int getCursorPosition(int *rows, int *cols) {
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

void yy_init() {
    get_term_size(&editor.rows, &editor.cols);
    term_clear();
    set_cursor(0, 0);
    draw_rows();
    editor.running = 1;
}

void yy_quit() {
    editor.running = 0;
    /* fclose(editor.file); */
    exit(1);
}

