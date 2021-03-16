#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

#include <term/mode.h>
#include <editor/state.h>

/* Setup terminal */
void term_setup() {
    /* Get current terminal's attributes */
    if(tcgetattr(STDIN_FILENO, &editor.orig) == -1) {
        exit(1);
    }

    /* Restore terminal on exit */
    atexit(term_reset);

    /* Short reference:
     *
     * c_lflag = local flags
     * c_iflag = input flags
     * c_oflag = output flags
     * c_cflag = control flags
     *
    */

    /* Turn off:
     *
     * echoing
     * canoncial mode
     * ctrl+c & ctrl+z
     * ctrl+s & ctrl+q signals
     * output processing("\n", "\r")
     *
    */

    struct termios termios = editor.orig;

    termios.c_iflag &= ~(IXON | ICRNL | ISTRIP | INPCK | BRKINT);
    termios.c_oflag &= ~(OPOST);
    termios.c_cflag |= CS8;
    termios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    /* Make `read` not wait indefinitely for input. */
    termios.c_cc[VMIN] = 0;
    termios.c_cc[VTIME] = 1;

    /* Set new attributes */
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios) == -1) {
        exit(1);
    }
}

/* When program exits, set the previous terminal settings */
void term_reset() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &editor.orig);
    fflush(stdout);
}
