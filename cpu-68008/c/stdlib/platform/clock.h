/**
 * Provides some routines for interacting with the hardware clock.
 */
#ifndef PLATFORM_CLOCK_H
#define PLATFORM_CLOCK_H

#include <stdint.h>

/**
 * Clock data structure
 */
typedef struct {
  // date component
  uint8_t century;
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t dayOfWeek;

  // time component
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t msecs;
} __attribute__((packed)) clock_t;



/**
 * Attempts to read the clock into the provided struct.
 */
int clock_read(clock_t *clock);

/**
 * Attempts to write to the hardware clock the provided time and date.
 */
int clock_write(clock_t *toWrite);

#endif
