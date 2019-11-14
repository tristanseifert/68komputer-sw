#ifndef SCREENMODE_H
#define SCREENMODE_H

#include <stdint.h>

/**
 * Defines the structure of a screen mode. Each of these can be thought of as a
 * separate "page" that the user can access.
 */
typedef struct {
  // pointer to title of mode, or null
  const char *title;
  //

  // called when the screen is about to appear
  void (*willAppear)();
  // called during the event loop
  void (*poll)();
  // called when the screen is about to disappear
  void (*willDisappear)();
} screen_mode_t;


// screen modes list
extern const screen_mode_t gScreenModes[];

#endif
