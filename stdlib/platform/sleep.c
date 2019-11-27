#include "sleep.h"

/**
 * Sleeps for the defined number of ms. This traps into the ROM.
 */
int sleep_ms(unsigned int ms) {
  // the trap expects increments of 10ms
  unsigned int ticks = (ms / 10);

  int ret = -1;
  __asm__ __volatile__(
    "\tmoveq #3, %%d0\n"
    "\tmove.l %1, %%d1\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : "g"(ticks) /* milliseconds */
    : "d0", "d1" /* clobber d0-d1 */
  );

  return ret;
}
