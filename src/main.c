#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <editor.h>
#include <term/mode.h>
#include <term/input.h>

int main(int argc, char ** argv) {

    if(argc != 2) {
        printf("Usage: yy <file>\n");
        return 1;
    }

    load_file(argv[1]);

    term_setup();
    render_state();

    while(editor.running) {
        handle_keys();
    }
}
