#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <editor.h>
#include <term/input.h>
#include <term/mode.h>

#define MIN(x, y) ((x) > (y) ? (y) : (x))

editor_state_t editor = {};

/* Render the terminal's state */
void render_state() {
    set_cursor(0, 0);
    term_clear();

    size_t line_display_count = MIN(editor.buffer.line_count, editor.term_height - 1);

    for(size_t i = 0; i < line_display_count; i++) {
        if(strlen(editor.buffer.lines[i + editor.buffer.vscroll]) < editor.buffer.hscroll) {
            printf("\r\n");
            continue;
        }

        char *line = editor.buffer.lines[i + editor.buffer.vscroll] + editor.buffer.hscroll;
        size_t line_length = strlen(line);

        if(line_length < editor.term_width)
            printf("%s\r\n", line);
        else if(line_length > editor.term_width)
            printf("%.*s>\r\n", editor.term_width - 1, line);
        else
            printf("\r\n");
    }

    printf("\x1b[%i;1H", editor.term_height);

    size_t cursor_display_x = editor.buffer.cursor_x - editor.buffer.hscroll;
    size_t cursor_display_y = editor.buffer.cursor_y - editor.buffer.vscroll;

    printf("\x1b[%zu;%zuH", cursor_display_y + 1, cursor_display_x + 1);
    fflush(stdout);
}

/* Load a file in to the buffer */
void load_file(char * file_path) {
    FILE *file = fopen(file_path, "r+");
    if(!file) {
        printf("Couldn't open file: %s\n", strerror(errno));
        exit(1);
    }

    char *line = NULL;
    size_t size, i = 0;
    ssize_t length;

    editor.buffer.lines = malloc(sizeof(char*));

    while((length = getline(&line, &size, file)) != -1) {
        length--;
        while(line[length] == '\n' || line[length] == '\r')
            line[length--] = 0;

        editor.buffer.lines = realloc(editor.buffer.lines, sizeof(char*) * (i + 1));
        editor.buffer.lines[i] = strdup(line);
        i++;
    }

    editor.buffer.line_count = i;
    editor.buffer.hscroll = 0;
    editor.buffer.vscroll = 0;
    editor.buffer.file = file;
    editor.buffer.name = basename(file_path);
    editor.buffer.modified = 0;
}
