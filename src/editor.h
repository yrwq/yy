#include <stddef.h>
#include <stdio.h>
#include <termios.h>

#define BIT(n) (1 << n)
#define MIN(x, y) ((x) > (y) ? (y) : (x))

enum editor_special_key {
    KEY_RETURN = 13,
    KEY_BACKSPACE = 127,
    KEY_ARROW_UP = 1000,
    KEY_ARROW_DOWN,
    KEY_ARROW_LEFT,
    KEY_ARROW_RIGHT
};

enum editor_key_modifier {
    MODIFIER_CTRL  = BIT(0),
    MODIFIER_SHIFT = BIT(1)
};

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
void load_file(char * file_path);
void insert_char(char c);
void delete_char();
void insert_line();
void quit();
void backspace();
void move_cursor(int x, int y, int render, int relative);
