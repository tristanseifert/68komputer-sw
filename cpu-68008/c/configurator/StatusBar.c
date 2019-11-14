#include "StatusBar.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/**
 * Internal status bar state
 */
static struct {
  // does the status bar need to be updated?
  bool barDirty;

  // keys to display
  uint8_t numEntries;
  status_bar_entry_t entries[8];
} gState;


static void status_bar_draw();


/**
 * Initializes the status bar.
 */
void status_init() {
  // clear internal state
  memset(&gState, 0, sizeof(gState));

  // draw the status bar (background)
  puts("\0337\033[24;1H\033[7m");

  for(uint8_t i = 0; i < 80; i++) {
    puts(" ");
  }

  puts("\033[0m\0338");
}

/**
 * Cleans up the status bar.
 */
void status_deinit() {

}


/*
 * Polls for status bar events.
 */
void status_poll() {
  // has the bar been dirtied?
  if(gState.barDirty) {
    status_bar_draw();
  }
}

/**
 * Draws the status bar
 */
static void status_bar_draw() {
  // go to (2, 24)
  puts("\0337\033[24;2H");

  // draw each of the entries
  for(uint8_t i = 0; i < gState.numEntries; i++) {
    status_bar_entry_t *entry = &gState.entries[i];

    // draw this entry
    printf("\033[0m%s\033[7m%s ", entry->key, entry->title);
  }

  // restore cursor position
  puts("\0338");

  // no longer dirty :)
  gState.barDirty = false;
}

/**
 * Sets the entries that are displayed on the status bar.
 */
int status_set(uint8_t num, const status_bar_entry_t *entries) {
  // copy the number of entries
  if(num > 8) {
    return -1;
  }

  gState.numEntries = num;

  // then, copy the actual entries
  for(uint8_t i = 0; i < num; i++) {
    memcpy(&gState.entries[i], &entries[i], sizeof(status_bar_entry_t));
  }

  // mark bar as dirty
  gState.barDirty = true;
}
