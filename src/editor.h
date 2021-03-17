#include <stddef.h>
#include <stdio.h>
#include <termios.h>

typedef struct editor_state {
    struct termios orig;
    int rows;
    int cols;
    int running;
} editor_state_t;

extern editor_state_t editor;

void insert_char(char c);
void yy_init();
void yy_quit();
