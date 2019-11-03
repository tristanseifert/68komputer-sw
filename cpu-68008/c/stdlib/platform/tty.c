/**
 * Provides some routines on interfacing with the TTY through the ROM API.
 */
#include <stdbool.h>
#include <stdint.h>

/**
 * Sets the TTY echo state
 */
int tty_set_echo(bool enabled) {
  int ret;
  uint32_t val = 0;

  if(enabled) {
    val |= 0x00000001;
  }

  // use trap $e, attrib is in d1, return code in d0
  __asm__ __volatile__(
    "\tmoveq #5, %%d0\n"
    "\tmove.l %1, %%d1\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : "g"(val) /* new tty state */
    : "d0", "d1" /* clobber d0-d1 */
  );

  // return value
  return ret;
}
