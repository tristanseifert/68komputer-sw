#include "MainScreen.h"

#include "StatusBar.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
    } state;

    // escape sequence buffer
    char escapeBuf[8];
    // total bytes read for the current state
    uint8_t escapeLen;
} gState;

// recognized escape sequences
const uint8_t  kEscF6[] = {0x31, 0x37, 0x7e};
const uint8_t  kEscF7[] = {0x31, 0x38, 0x7e};
const uint8_t  kEscF8[] = {0x31, 0x39, 0x7e};
const uint8_t  kEscF9[] = {0x32, 0x30, 0x7e};
const uint8_t kEscF10[] = {0x32, 0x31, 0x7e};
const uint8_t kEscF11[] = {0x32, 0x33, 0x7e};
const uint8_t kEscF12[] = {0x32, 0x34, 0x7e};
const uint8_t kEscF13[] = {0x32, 0x35, 0x7e};
const uint8_t kEscF14[] = {0x32, 0x36, 0x7e};
const uint8_t kEscF15[] = {0x32, 0x38, 0x7e}; // help
const uint8_t kEscF16[] = {0x32, 0x39, 0x7e}; // do
const uint8_t kEscF17[] = {0x33, 0x31, 0x7e};
const uint8_t kEscF18[] = {0x33, 0x32, 0x7e};
const uint8_t kEscF19[] = {0x33, 0x33, 0x7e};
const uint8_t kEscF20[] = {0x33, 0x34, 0x7e};



/**
 * "About to appear" function for main screen
 */
void scr_main_appear() {
  // clear state
  memset(&gState, 0, sizeof(gState));

  // go cursor to top left of user area
  puts("\033[2;1H");

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
       "associated key. To exit, type Ctrl+C.");

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

          }
          // F7? (serial settings)
          else if(memcmp(&gState.escapeBuf, &kEscF7, 3) == 0) {

          }
          // F8? (expansion settings)
          else if(memcmp(&gState.escapeBuf, &kEscF8, 3) == 0) {

          }
          // F9? (reset to defaults)
          else if(memcmp(&gState.escapeBuf, &kEscF9, 3) == 0) {

          }
          // unknown escape sequence
          else {
            puts("\a");
          }

          // printf("\033[16;4H Sequence: %#04x %#04x %#04x (%c%c%c)", gState.escapeBuf[0], gState.escapeBuf[1], gState.escapeBuf[2], gState.escapeBuf[0], gState.escapeBuf[1], gState.escapeBuf[2]);

          // go back to idle state
          gState.state = kStateIdle;
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
