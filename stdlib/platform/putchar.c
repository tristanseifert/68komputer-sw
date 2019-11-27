/**
 * Prints a character to the UART. This is done via an IO trap.
 */
#include <stdio.h>

/**
 * Prints a single character to the TTY
 */
int putchar(int c) {
  volatile const char v = (const char) c;

  // use trap $8, character should be in d1
  __asm__ __volatile__(
    "\tmoveq #8, %%d0\n"
    "\tmove.b %0, %%d1\n"
    "\ttrap #1\n"
    : /* no outputs */
    : "g"(v) /* input the character */
    : "d0", "d1" /* clobber a0, d0-d1 */
  );

  // always success
  return c;
}

/**
 * putchar wrapper for printf()
 */
void _putchar(char character) {
  putchar(character);
}
