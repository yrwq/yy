#include <stddef.h>
#include <stdio.h>
#include <termios.h>

#define CTRL_KEY(k) ((k) & 0x1f)

int read_key();
void handle_keys();
void move_cursor_normal(char key);
