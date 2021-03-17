#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include <term/input.h>
#include <term/mode.h>
#include <editor.h>

editor_state_t editor = {};

void insert_char(char c) {
    printf("%c", c);
    fflush(stdout);
}

void yy_init() {
    term_clear();
    set_cursor(0, 0);
    editor.running = 1;
}

void yy_quit() {
    editor.running = 0;
    /* fclose(editor.file); */
    exit(1);
}
