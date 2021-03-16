#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <term/mode.h>
#include <term/input.h>

int main(int argc, char ** argv) {
    /* Setup terminal */

    term_clear();
    set_cursor(0,0);

    term_setup();

    while(true){ handle_keys(); }
}
