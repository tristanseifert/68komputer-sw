/**
 * Prints a character to the UART. This is done via an IO trap.
 */
#include <stdio.h>

/**
 * Reads a single character from the TTY
 */
int getchar() {
  int c;

  // use trap $9, received character will be in d0
  __asm__ __volatile__(
    "\tmoveq #9, %%d0\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(c) /* outputs the character */
    : /* no inputs */
    : "d0", "d1" /* clobber a0, d0-d1 */
  );

  // always success
  return (c & 0xFF);
}
