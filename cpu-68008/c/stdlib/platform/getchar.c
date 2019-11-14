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
    "\tmoveq #0, %%d1\n"
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

/**
 * Reads a single character from the TTY, without blocking. If a character was
 * available, it is returned; otherwise, -1 is returned.
 */
int getchar_nonblocking() {
  int c;

  // use trap $a, received character will be in d0
  __asm__ __volatile__(
    "\tmoveq #1, %%d1\n"
    "\tmoveq #9, %%d0\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(c) /* outputs the character */
    : /* no inputs */
    : "d0", "d1" /* clobber a0, d0-d1 */
  );

  // always success
  return c;
}
