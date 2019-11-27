#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <stdint.h>

/**
 * A single status bar entry
 */
typedef struct {
  // key for this action
  const char *key;
  // title for this action
  const char *title;
} status_bar_entry_t;

/**
 * Initializes the status bar.
 */
void status_init();
/**
 * Cleans up the status bar.
 */
void status_deinit();


/*
 * Polls for status bar events.
 */
void status_poll();


/**
 * Sets the entries that are displayed on the status bar.
 */
int status_set(uint8_t num, const status_bar_entry_t *entries);

#endif
