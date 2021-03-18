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

    yy_init();

    while(editor.running) {
        yy_refresh();
        handle_keys();
    }
}
