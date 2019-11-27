/**
 * The title bar is 80 column line at the top of the terminal, e.g. line 1. It
 * shows some standard information, like the title of the current page, and a
 * clock.
 */
#ifndef TITLEBAR_H
#define TITLEBAR_H

/**
 * Initialize the title bar.
 */
void title_init();
/**
 * Cleans up the title bar state.
 */
void title_deinit();



/**
 * Polls for events: this handles automagically updating the time.
 */
void title_poll();



/**
 * Sets the title displayed on the bar.
 */
void title_set(const char *title);

#endif
