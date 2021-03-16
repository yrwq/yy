#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

#include <term/mode.h>

struct termios orig;

/* Setup terminal */
void term_setup() {
    /* Get current terminal's attributes */
    tcgetattr(STDIN_FILENO, &orig);

    /* Restore terminal on exit */
    atexit(term_reset);

    struct termios raw_mode;

    raw_mode = orig;

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

    raw_mode.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw_mode.c_oflag &= ~(OPOST);
    raw_mode.c_cflag |= (CS8);
    raw_mode.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /*
     *
     * Sets read() function not to pause,
    untill there is user input,
    instead it waits for a counple of milliseconds
    and then the loop proceeds
    *
    */

    raw_mode.c_cc[VMIN] = 0;
    raw_mode.c_cc[VTIME] = 1;

    /* Set new attributes */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode);
}

/* When program exits, set the previous terminal settings */
void term_reset() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
}
