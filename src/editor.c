#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include <term/input.h>
#include <term/mode.h>
#include <editor.h>

editor_state_t editor = {};

/* Render the terminal */
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
            printf("%.*s\r\n", editor.term_width - 1, line);
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

/* Insert a character after the cursor */
void insert_char(char c) {
    size_t x = editor.buffer.cursor_x;
    size_t y = editor.buffer.cursor_y;
    size_t line_length = strlen(editor.buffer.lines[y]);
    editor.buffer.lines[y] = realloc(editor.buffer.lines[y], line_length + 2);
    memset(editor.buffer.lines[y] + line_length, 0, 2);

    memmove(editor.buffer.lines[y] + x,
            editor.buffer.lines[y] + x - 1,
            line_length - x + 1);

    editor.buffer.lines[y][x] = c;
    editor.buffer.cursor_x++;
    editor.buffer.modified = 1;

    render_state();
}

/* Insert a line after the cursor */
void insert_line() {
    size_t x = editor.buffer.cursor_x;
    size_t y = editor.buffer.cursor_y;

    editor.buffer.lines = realloc(editor.buffer.lines,
                                  sizeof(char*) * editor.buffer.line_count + 1);

    for(size_t i = editor.buffer.line_count; i > y; i--) {
        editor.buffer.lines[i] = editor.buffer.lines[i - 1];
    }

    size_t remaining_line_length = strlen(editor.buffer.lines[y]) - x;

    editor.buffer.lines[y + 1] = malloc(remaining_line_length + 1);
    editor.buffer.lines[y + 1][remaining_line_length] = 0;

    memmove(editor.buffer.lines[y + 1],
            editor.buffer.lines[y] + x,
            remaining_line_length);

    memset(editor.buffer.lines[y] + x, 0, remaining_line_length);

    move_cursor(0, 1, 1, 1);

    editor.buffer.line_count++;
    editor.buffer.modified = 1;

    render_state();
}

/* Move the cursor */
void move_cursor(int x, int y, int render, int relative) {
    size_t new_x = x;
    size_t new_y = y;

    if(relative) {
        new_x += editor.buffer.cursor_x;
        new_y += editor.buffer.cursor_y;
    }

    if(new_x >= 0) {
        editor.buffer.cursor_x = new_x;
    }

    if(new_y >= 0
       && new_y <= editor.buffer.line_count - 1) {
        if(new_y >= editor.term_height + editor.buffer.vscroll - 1) {
            editor.buffer.vscroll++;
        } else if(new_y < editor.buffer.vscroll) {
            editor.buffer.vscroll--;
        }

        editor.buffer.cursor_y = new_y;

        size_t cursor_x_limit = strlen(editor.buffer.lines[new_y]);

        editor.buffer.cursor_x = MIN(editor.buffer.cursor_x, cursor_x_limit);
    }

    if(editor.buffer.cursor_x >= editor.term_width + editor.buffer.hscroll - 1) {
        editor.buffer.hscroll += editor.term_width - 1;
    } else if(editor.buffer.cursor_x < editor.buffer.hscroll) {
        editor.buffer.hscroll -= editor.term_width - 1;
    }

    if(render)
        render_state();
}

/* Delete a character */
void delete_char() {
    size_t x = editor.buffer.cursor_x;
    size_t y = editor.buffer.cursor_y;
    size_t line_length = strlen(editor.buffer.lines[y]);

    if(x == 0 && y == 0)
        return;

    if(x > 0) {
        memmove(editor.buffer.lines[y] + x - 1,
                editor.buffer.lines[y] + x,
                line_length - x + 1);

        editor.buffer.cursor_x--;
    } else {
        size_t previous_line_length = strlen(editor.buffer.lines[y - 1]);
        size_t total_line_size = previous_line_length + line_length + 1;
        editor.buffer.lines[y - 1] = realloc(editor.buffer.lines[y - 1],
                                             total_line_size);

        memmove(editor.buffer.lines[y - 1] + previous_line_length,
                editor.buffer.lines[y],
                line_length);

        for(size_t i = y + 1; i < editor.buffer.line_count; i++) {
            editor.buffer.lines[i - 1] = editor.buffer.lines[i];
        }

        editor.buffer.line_count--;
        editor.buffer.cursor_x = total_line_size - 1;

        move_cursor(0, -1, 1, 1);
    }

    editor.buffer.modified = 1;

    render_state();
}

/* Quit the program */
void quit() {
    fclose(editor.buffer.file);
    editor.running = 0;
    exit(1);
}

