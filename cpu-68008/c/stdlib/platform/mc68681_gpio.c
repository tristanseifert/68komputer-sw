#include "mc68681_gpio.h"

#include <stdio.h>

/**
 * IO traps for the 68681 GPIOs.
 */
int gpio_read(void) {
  int value;

  // use trap $4, input state is in d0
  __asm__ __volatile__(
    "\tmoveq #4, %%d0\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(value) /* outputs the state */
    : /* no inputs */
    : "d0", "d1" /* clobber d0-d1 */
  );

  return value;
}

/**
 * Sets the state of the 68681 GPIOs.
 */
int gpio_set(uint8_t state) {
  int ret;

  // get a 16-bit word containing the bits to set
  uint16_t toSet = state;
  // then, a 16-bit word containing bits to clear
  uint16_t toClear = ~state;
  // combine it into the argument for the trap
  uint32_t arg = (toClear << 16) | (toSet);

  // printf("Set: $%08x\r\n", arg);

  // use trap $5, desired output is in d1
  __asm__ __volatile__(
    "\tmoveq #5, %%d0\n"
    "\tmove.l %1, %%d1\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : "g"(arg) /* new gpio state */
    : "d0", "d1" /* clobber d0-d1 */
  );

  return ret;
}
