#include "ScreenMode.h"

#include "MainScreen.h"
#include "ClockScreen.h"

#include <stddef.h>

/**
 * Empty (useless) callback
 */
static void doNothing() {}

/**
 * This is the global array of screen modes.
 */
const screen_mode_t gScreenModes[] = {
  // init mode
  {
    // no title
    .title = NULL,

    // define some empty callbacks
    .willAppear = doNothing,
    .poll = doNothing,
    .willDisappear = doNothing
  },

  // home page
  {
    .title = "Configurator",

    .willAppear = scr_main_appear,
    .poll = scr_main_poll,
    .willDisappear = scr_main_disappear
  },
  // clock settings
  {
    .title = "Clock Settings",

    .willAppear = scr_clock_appear,
    .poll = scr_clock_poll,
    .willDisappear = scr_clock_disappear
  },
};
