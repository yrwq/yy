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
        switch(editor.mode) {
            case 1:
                handle_insert_keys();
            case 2:
                handle_normal_keys();
            default:
                handle_normal_keys();
        }
    }
}
