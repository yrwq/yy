#include <stddef.h>
#include <stdio.h>
#include <termios.h>

int read_key();
void move_cursor(char key);
void handle_normal_keys();
void handle_insert_keys();
