#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <editor.h>
#include <term/mode.h>
#include <term/input.h>

int main(int argc, char ** argv) {

    yy_init();

    if(argc != 2) {
        printf("Usage: yy <file>\n");
        return 1;
    } else {
        load_file(argv[1]);
    }

    while(true) {
        yy_refresh();
    }

}
