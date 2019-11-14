#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "ScreenMode.h"
#include "TitleBar.h"
#include "StatusBar.h"

#include <platform/tty.h>

static uint8_t currentScreen = 0;
static uint8_t nextScreen = 0;

/**
 * Entry point for the configurator application.
 */
int main() {
  uint8_t run = 1;

  // set up the terminal (clear)
  puts("\033[2J");
  tty_set_echo(false);

  // set up the various components
  title_init();
  status_init();

  // set up for the home screen
  nextScreen = 1;

  // this is our main input loop
  while(run) {
    // give all components a chance to poll for events
    title_poll();
    status_poll();

    gScreenModes[currentScreen].poll();

    // changing screen mode?
    if(currentScreen != nextScreen) {
      // call the "about to disappear" callback of the old screen
      if(gScreenModes[currentScreen].willDisappear != NULL) {
        gScreenModes[currentScreen].willDisappear();
      }

      // update the screen mode value and titlebar title
      currentScreen = nextScreen;

      if(gScreenModes[currentScreen].title != NULL) {
        title_set(gScreenModes[currentScreen].title);
      }

      // we now clear from the first line down and re-init status bar
      puts("\033[?25l\033[2;1H\033[J");
      status_init();

      // call the "about to appear" callback of the new screen
      if(gScreenModes[currentScreen].willAppear != NULL) {
        gScreenModes[currentScreen].willAppear();
      }
    }
  }

  // clean up the various components
  title_deinit();
  status_deinit();

  // lastly, clean up the terminal (clear + reset)
  puts("\033[2J\033c");
  tty_set_echo(true);

  return 0;
}
