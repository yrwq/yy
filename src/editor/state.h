#include <stddef.h>
#include <stdio.h>
#include <termios.h>

typedef struct editor_state {
    struct termios orig;
    int term_width, term_height;
    int running;
} editor_state_t;

extern editor_state_t editor;
