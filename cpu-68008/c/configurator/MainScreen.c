#include "MainScreen.h"
#include "ScreenMode.h"
#include "StatusBar.h"

#include "tty.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * Status bar keys
 */
static const status_bar_entry_t gStatusEntries[] = {
  {
    .key = "F6",
    .title ="Clock"
  },
  {
    .key = "F7",
    .title ="Serial"
  },
  {
    .key = "F8",
    .title ="Expansion"
  },
  {
    .key = "F9",
    .title ="Defaults"
  },
  {
    .key = "F10",
    .title ="Exit"
  },
};


static const status_bar_entry_t gStatusEntriesConfirm[] = {
 {
   .key = "Y",
   .title ="Confirm"
 },
 {
   .key = "N",
   .title ="Abort"
 },
};

/**
 * State
 */
static struct {
    // state machine state
    enum {
      // wait for a key press
      kStateIdle,
      // when receiving $9b
      kStateReadFunctionKey,
      // respond to the "reset to defaults" message
      kStateWaitResetAnswer,
    } state;

    // escape sequence buffer
    char escapeBuf[8];
    // total bytes read for the current state
    uint8_t escapeLen;

    // are we in the "reset to defaults" mode?
    bool inDefaults;
} gState;



/**
 * "About to appear" function for main screen
 */
void scr_main_appear() {
  // clear state
  memset(&gState, 0, sizeof(gState));

  // print banner
  puts(""
  "  ____  _____ _                               _\r\n"
  " / ___||  _  | |                             | |\r\n"
  "/ /___  \\ V /| | _____  _ __ ___  _ __  _   _| |_ ___ _ __\r\n"
  "| ___ \\ / _ \\| |/ / _ \\| '_ ` _ \\| '_ \\| | | | __/ _ \\ '__|\r\n"
  "| \\_/ || |_| |   < (_) | | | | | | |_) | |_| | ||  __/ |\r\n"
  "\\_____/\\_____/_|\\_\\___/|_| |_| |_| .__/ \\__,_|\\__\\___|_|\r\n"
  "                                 | |\r\n"
  "                                 |_|\r\n"
  "Configurator (Version 1.0)\r\n");

  // print the directions
  puts("Select any of the options shown at the bottom of the screen by entering the\r\n"
       "associated key. To exit, enter F10.");

  // set up the status bar
  status_set(sizeof(gStatusEntries)/sizeof(status_bar_entry_t), gStatusEntries);
}


/**
 * Input polling function for main screen
 */
void scr_main_poll() {
  int key;
  static int col = 1;

  // check if we've received a keypress
  key = getchar_nonblocking();

  if(key != -1) {
    switch(gState.state) {
      // idle state
      case kStateIdle: {
        // received $9b (function key)?
        if((key & 0xff) == 0x9b) {
          gState.state = kStateReadFunctionKey;
          gState.escapeLen = 0;
        }

        break;
      }

      // reading a function key: two ASCII chars, then ~
      case kStateReadFunctionKey: {
        // store the key
        gState.escapeBuf[gState.escapeLen++] = (key & 0xff);

        // if we've read three keys, see what we got
        if(gState.escapeLen == 3) {
          // F6? (clock settings)
          if(memcmp(&gState.escapeBuf, &kEscF6, 3) == 0) {
            scr_set(2);
          }
          // F7? (serial settings)
          else if(memcmp(&gState.escapeBuf, &kEscF7, 3) == 0) {

          }
          // F8? (expansion settings)
          else if(memcmp(&gState.escapeBuf, &kEscF8, 3) == 0) {

          }
          // F9? (reset to defaults)
          else if(memcmp(&gState.escapeBuf, &kEscF9, 3) == 0) {
            // print confirmation message
            puts("\033[20;2H\033[7mAre you sure? All settings will be reset!\033[0m");

            status_set(sizeof(gStatusEntriesConfirm)/sizeof(status_bar_entry_t), gStatusEntriesConfirm);
            gState.state = kStateWaitResetAnswer;
          }
          // F10? exit
          else if(memcmp(&gState.escapeBuf, &kEscF10, 3) == 0) {
            scr_exit();
          }
          // unknown escape sequence
          else {
            puts("\a");
          }

          // go to the idle mode if we haven't changed to another one already
          if(gState.state == kStateReadFunctionKey) {
            gState.state = kStateIdle;
          }
        }

        break;
      }

      // handle the reset to defaults mode
      case kStateWaitResetAnswer: {
        // convert character to uppercase
        key = toupper(key);

        // yes?
        if(key == 'Y') {
          // TODO: reset

          // print a message
          puts("\033[20;2H\033[K\033[20;2H\033[7mSuccessfully reset to defaults\033[0m");

          gState.state = kStateIdle;
          gState.inDefaults = false;
        }
        // nope
        else if(key == 'N') {
          // clear the line
          puts("\033[20;2H\033[K");

          gState.state = kStateIdle;
          gState.inDefaults = false;
        }
        // any other key (just beep)
        else {
          puts("\a");
        }

        // if we've finished, restore the old status bar
        if(gState.state != kStateWaitResetAnswer) {
          status_set(sizeof(gStatusEntries)/sizeof(status_bar_entry_t), gStatusEntries);
        }

        break;
      }

      // unknown state, shouldn't get here
      default:
        break;
    }

    // printf("\033[15;%dH$%02x", col, key);
    // col += 4;
  }
}

/**
 * "About to disappear" function for main screen
 */
void scr_main_disappear() {

}
