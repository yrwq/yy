#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <editor.h>
#include <term/mode.h>
#include <term/input.h>

int main(int argc, char ** argv) {

    term_setup();
    render_state();

    while(true){ handle_keys(); }
}
