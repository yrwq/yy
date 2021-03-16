#include <stddef.h>
#include <stdio.h>
#include <termios.h>

#define BIT(n) (1 << n)

typedef struct editor_buffer {
    char ** lines;
    size_t line_count;
    size_t cursor_x, cursor_y;
    size_t hscroll, vscroll;
    FILE * file;
    char * name;
    int modified;
} editor_buffer_t;

typedef struct editor_state {
    struct termios orig;
    int term_width, term_height;
    int running;
    editor_buffer_t buffer;
} editor_state_t;

extern editor_state_t editor;

void render_state();
