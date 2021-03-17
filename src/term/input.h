#include <stddef.h>
#include <stdio.h>
#include <termios.h>

#define CTRL_KEY(k) ((k) & 0x1f)

void term_clear();
void set_cursor(int x, int y);

int read_key(size_t *modifiers_out);
void handle_keys();
