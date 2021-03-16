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
#include <term/mode.h>

#define MIN(x, y) ((x) > (y) ? (y) : (x))

editor_state_t editor = {};
