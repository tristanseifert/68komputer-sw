#ifndef PLATFORM_TTY_H
#define PLATFORM_TTY_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Sets the TTY echo state
 */
int tty_set_echo(bool enabled);

#endif
