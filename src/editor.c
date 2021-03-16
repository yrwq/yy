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

    printf("\x1b[%zu;1H", editor.term_height);

    size_t cursor_display_x = editor.buffer.cursor_x - editor.buffer.hscroll;
    size_t cursor_display_y = editor.buffer.cursor_y - editor.buffer.vscroll;

    printf("\x1b[%zu;%zuH", cursor_display_y + 1, cursor_display_x + 1);
    fflush(stdout);
}
