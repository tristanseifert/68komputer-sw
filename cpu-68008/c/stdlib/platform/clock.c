#include "clock.h"

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/**
 * Helper function to convert an 8-bit BCD quantity
 */
static inline uint8_t bcd_to_bin(uint8_t bcd) {
    return (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F));
}

/**
 * Converts a binary value to BCD
 */
static inline uint8_t bin_to_bcd(uint8_t bin) {
  uint8_t temp = 0;

  while(bin > 9) {
    temp += 0x10;
    bin -= 10;
  }

  return (bin + temp);
}



/**
 * Attempts to read the clock into the provided struct.
 */
int clock_read(clock_t *clock) {
  if(clock == NULL) return -1;

  // read from the clock
  int ret = -1;
  __asm__ __volatile__(
    "\tmoveq #6, %%d0\n"
    "\tmove.l %1, %%a1\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : "g"(clock) /* clock struct */
    : "d0", "a1" /* clobber d0, a1 */
  );

  // convert from BCD to binary
  if(ret == 0) {
    clock->century = bcd_to_bin(clock->century);
    clock->year = bcd_to_bin(clock->year);
    clock->month = bcd_to_bin(clock->month);
    clock->day = bcd_to_bin(clock->day);

    // clock->dayOfWeek = bcd_decimal(clock->dayOfWeek);
    clock->dayOfWeek &= 0x07;

    clock->hours = bcd_to_bin(clock->hours);
    clock->minutes = bcd_to_bin(clock->minutes);
    clock->seconds = bcd_to_bin(clock->seconds);
    clock->msecs = bcd_to_bin(clock->msecs);
  }

  return ret;
}

/**
 * Attempts to write to the hardware clock the provided time and date.
 */
int clock_write(clock_t *clock) {
  if(clock == NULL) return -1;

  // convert the clock struct to BCD
  clock->century = bin_to_bcd(clock->century);
  clock->year = bin_to_bcd(clock->year);
  clock->month = bin_to_bcd(clock->month);
  clock->day = bin_to_bcd(clock->day);

  // printf("%02x %02x %02x\r\n", clock->year, clock->month, clock->day);

  // clock->dayOfWeek = bcd_decimal(clock->dayOfWeek);
  clock->dayOfWeek &= 0x07;

  clock->hours = bin_to_bcd(clock->hours);
  clock->minutes = bin_to_bcd(clock->minutes);
  clock->seconds = bin_to_bcd(clock->seconds);
  clock->msecs = bin_to_bcd(clock->msecs);

  // write to the clock
  int ret = -1;
  __asm__ __volatile__(
    "\tmoveq #7, %%d0\n"
    "\tmove.l %1, %%a1\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : "g"(clock) /* clock struct */
    : "d0", "a1" /* clobber d0, a1 */
  );

  return ret;
}


/**
 * Gets the value of the system tick counter. This is the number of 100ths of
 * seconds since the system has been powered on.
 */
unsigned int clock_get_ticks() {
  int ret = -1;
  __asm__ __volatile__(
    "\tmoveq #0x10, %%d0\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : /* no inputs */
    : "d0" /* clobber d0 */
  );

  return ret;
}
