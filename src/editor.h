#include <stddef.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

typedef struct editor_state {
    struct termios orig;
    int cx, cy; // Cursor x, y
    int rows;
    int cols;
    int running;
    int mode;   // 0 = normal, 1 insert
} editor_state_t;

extern editor_state_t editor;

struct abuf {
    char * b;
    int len;
};

#define ABUF_INIT {NULL, 0}

void insert_char(char c);
void yy_init();
void yy_quit();
void yy_refresh();
void draw_rows(struct abuf * ab);
void buf_append(struct abuf * ab, const char * s, int len);
void buf_free(struct abuf * ab);
