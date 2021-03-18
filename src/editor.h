#include <stddef.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

typedef struct erow {
    int size;
    char * chars;
} erow;

typedef struct editor_state {
    struct termios orig;
    int cx, cy; // Cursor x, y
    int rowoff;
    int coloff;
    int rows;
    int cols;
    int running;
    int mode;
    int numrows;
    erow * row;
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
void load_file(char * filename);
void append_row(char * s, size_t len);
void scrolloff();
void detect_mode();
