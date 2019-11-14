#include "TitleBar.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <string.h>

#include <platform/clock.h>

/**
 * Short names for days
 */
static const char* gDayShortNames[] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat"
};

/**
 * Internal title bar state
 */
static struct {
  // do we need to output the full clock line?
  bool outputFullClock;
  // at which time value the clock should be updated
  unsigned int clock;

  // does the title need to be updated?
  bool titleDirty;
  // title to display in the bar
  char title[32];
} gState;


static void title_clock_draw();



/**
 * Initialize the title bar.
 */
void title_init() {
  // zero our internal state
  memset(&gState, 0, sizeof(gState));

  // update clock immediately
  gState.clock = clock_get_ticks();
  gState.outputFullClock = true;

  // draw the title bar (background)
  puts("\033[1;1H\033[7m");

  for(uint8_t i = 0; i < 80; i++) {
    puts(" ");
  }

  puts("\033[0m");
}
/**
 * Cleans up the title bar state.
 */
void title_deinit() {

}



/**
 * Polls for events: this handles automagically updating the time.
 */
void title_poll() {
  // do we need to update the clock?
  if(gState.clock <= clock_get_ticks()) {
    // update clock
    title_clock_draw();

    // update it again in a second (100 ticks)
    gState.clock = clock_get_ticks() + 100;
  }

  // do we need to update the title?
  if(gState.titleDirty) {
    // go to the position and draw the title
    printf("\033[1;1H\033[7m %.32s", gState.title);

    // title no longer dirty :)
    gState.titleDirty = false;
  }
}



/**
 * Sets the title displayed on the bar.
 */
void title_set(const char *title) {
  // copy as much as we can
  strncpy(gState.title, title, sizeof(gState.title));
  gState.titleDirty = true;
}


/**
 * Draws the clock.
 */
static void title_clock_draw() {
  // read clock
  clock_t clock;
  int err = clock_read(&clock);

  if(err != 0) {
    // handle clock read errors
    return;
  }

  // prepare for output
  const char *dayName = gDayShortNames[clock.dayOfWeek & 7];
  char timeSep = ((clock.seconds & 0x01) == 0) ? ':' : ' ';

  // update the full line if minutes are zero or requested
  if(clock.minutes == 0 || gState.outputFullClock) {
    printf("\033[1;57H\033[7m%s %02u-%02u-%02u%02u %02u%c%02u%c%02u\033[0m", dayName, clock.day, clock.month, clock.century, clock.year, clock.hours, timeSep, clock.minutes, timeSep, clock.seconds);

    gState.outputFullClock = false;
  }
  // just update the time otherwise
  else {
    printf("\033[1;72H\033[7m%02u%c%02u%c%02u\033[0m", clock.hours, timeSep, clock.minutes, timeSep, clock.seconds);
  }
}
