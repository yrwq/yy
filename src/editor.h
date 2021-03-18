#include <stddef.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

/* How much space will a tab take */
#define TAB_STOP 4

typedef struct erow {
    int size;
    int rsize;
    char * chars;
    char * render;
} erow;

typedef struct editor_state {
    struct termios orig;
    int cx, cy; // Cursor x, y
    int rx;
    int rowoff;
    int coloff;
    int rows;
    int cols;
    int running;
    int mode;
    int numrows;
    char * filename;
    char message[80];

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
void update_row(erow * row);
int row_cx_to_rx(erow * row, int cx);
void draw_bar(struct abuf * ab);
void set_bar_msg(const char * fmt, ...);
void insert_row(erow * row, int at, int c);
