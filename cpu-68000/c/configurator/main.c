#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "ScreenMode.h"
#include "TitleBar.h"
#include "StatusBar.h"

#include <platform/tty.h>
#include <platform/sleep.h>

static uint8_t currentScreen = 0;
static uint8_t nextScreen = 0;
static bool forceRedraw = false;

static uint8_t run = 1;

/**
 * Entry point for the configurator application.
 */
int main() {
  run = 1;

  // set up the terminal (clear)
  puts("\033[2J");
  tty_set_echo(false);

  // set up the various components
  title_init();
  status_init();

  // set up for the home screen
  nextScreen = 1;
  forceRedraw = false;

  // this is our main input loop
  while(run) {
    // give all components a chance to poll for events
    title_poll();
    status_poll();

    gScreenModes[currentScreen].poll();

    // changing screen mode?
    if(currentScreen != nextScreen || forceRedraw) {
      // disable echo at this point
      tty_set_echo(false);

      // call the "about to disappear" callback of the old screen
      if(gScreenModes[currentScreen].willDisappear != NULL) {
        gScreenModes[currentScreen].willDisappear();
      }

      // update the screen mode value and titlebar title
      currentScreen = nextScreen;
      forceRedraw = false;

      if(gScreenModes[currentScreen].title != NULL) {
        title_set(gScreenModes[currentScreen].title);
      }

      // we now clear from the first line down and re-init status bar
      puts("\033[?25l\033[2;1H");

      for(int i = 0; i < 22; i++) {
        puts("\033[K\n");
      }

      status_init();

      // call the "about to appear" callback of the new screen
      if(gScreenModes[currentScreen].willAppear != NULL) {
        // move cursor to top left of user area
        puts("\033[2;1H");
        gScreenModes[currentScreen].willAppear();
      }
    }
  }

  // clean up the various components
  title_deinit();
  status_deinit();

  // lastly, clean up the terminal (clear + reset)
  puts("\033[2J\033[1;1H");
  sleep_ms(200);

  tty_set_echo(true);
  // puts("Configurator Version 1.0\r\n");

  return 0;
}

/**
 * Changes the screen mode.
 */
void scr_set(uint8_t mode) {
  nextScreen = mode;
}

/**
 * Exit the program
 */
void scr_exit() {
  run = 0;
}
