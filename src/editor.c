#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <term/input.h>
#include <term/mode.h>
#include <editor.h>

editor_state_t editor = {};

void draw_rows() {
    int y;
    for (y = 0; y < 24; y++) {
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

void yy_init() {
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

