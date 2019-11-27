#include "MainScreen.h"
#include "ScreenMode.h"
#include "StatusBar.h"

#include "tty.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <platform/tty.h>
#include <platform/clock.h>


/**
 * Names of the days of the week
 */
static const char* gDayNames[] = {
 "Sunday",
 "Monday",
 "Tuesday",
 "Wednesday",
 "Thursday",
 "Friday",
 "Saturday",
};

/**
 * Status bar keys
 */
static const status_bar_entry_t gStatusEntries[] = {
  {
    .key = "F6",
    .title ="Save Changes"
  },
  {
    .key = "F10",
    .title ="Discard"
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
      // wait for any sort of keypress, then blank the error line
      kStateAfterError,
    } state;

    // escape sequence buffer
    char escapeBuf[8];
    // total bytes read for the current state
    uint8_t escapeLen;

    // current row of the form
    uint8_t formRow;

    // whether the cursor needs to be repositioned
    bool cursorDirty;
    // whether the form contents need to be redrawn
    bool formValuesDirty;

    // form values
    uint8_t values[6];

    // is the line buffer dirty?
    bool lineBufDirty;
    // character offset into line buffer
    uint8_t lineBufOffset;
    // read buffer for the current line
    char lineBuf[4];
} gState;


static void update_cursor();
static void draw_form_values();

static int save_clock_date();

static int get_day_of_week(uint8_t d, uint8_t m, uint8_t y);

static void convert_line();
static void prepare_line();



/**
 * "About to appear" function for clock screen
 */
void scr_clock_appear() {
  // clear state
  memset(&gState, 0, sizeof(gState));

  // load initial values
  clock_t clock;
  int ret = clock_read(&clock);

  if(ret == 0) {
    gState.values[0] = clock.year;
    gState.values[1] = clock.month;
    gState.values[2] = clock.day;

    gState.values[3] = clock.hours;
    gState.values[4] = clock.minutes;
    gState.values[5] = clock.seconds;
  }

  prepare_line();

  // print directions
  puts("Use TAB to move between fields. Day-of-week will update automatically.\r\n");

  // print the form
  puts(
    "\r\n\033[7mSet Time and Date\033[0m\r\n"
    "   Year:\r\n"
    "  Month:\r\n"
    "    Day:\r\n"
    "  Hours:\r\n"
    "Minutes:\r\n"
    "Seconds:"
  );

  // set up the status bar
  status_set(sizeof(gStatusEntries)/sizeof(status_bar_entry_t), gStatusEntries);

  // ensure we redraw everything next iteration
  gState.cursorDirty = true;
  gState.formValuesDirty = true;
}


/**
 * Input polling function for clock screen
 */
void scr_clock_poll() {
  int key;

  // check if we've received a keypress
  key = getchar_nonblocking();

  if(key != -1) {
    key &= 0xff;

    switch(gState.state) {
      // idle state
      case kStateIdle: {
        // received $9b (function key)?
        if(key == 0x9b) {
          gState.state = kStateReadFunctionKey;
          gState.escapeLen = 0;
        }
        // is it a tab? (move between fields)
        else if(key == '\t') {
          // try to convert the buffer
          convert_line();

          // go to the next row
          gState.formRow++;

          if(gState.formRow >= 6) {
            gState.formRow = 0;
          }

          prepare_line();
          gState.cursorDirty = true;
        }
        // is it a backspace?
        else if(key == '\b') {
          if(gState.lineBufOffset) {
            gState.lineBufOffset--;
            gState.lineBuf[gState.lineBufOffset] = '\0';

            // move the cursor back a position
            puts("\b");
          }

          gState.lineBufDirty = true;
        }
        // store all other numeric keys
        else if(isdigit(key)) {
          // store the character
          gState.lineBuf[gState.lineBufOffset] = key;

          // store at the next offset if we're not already at the end of array
          if(gState.lineBufOffset < 1) {
            gState.lineBufOffset++;
          }
          // if we are, move the cursor back
          else {
            puts("\b");
          }

          gState.lineBufDirty = true;
        }
        break;
      }

      // reading a function key: two ASCII chars, then ~
      case kStateReadFunctionKey: {
        gState.escapeBuf[gState.escapeLen++] = key;
        if(gState.escapeLen == 3) {
          // F6? (save changes)
          if(memcmp(&gState.escapeBuf, &kEscF6, 3) == 0) {
            // if line is dirty, save it
            if(gState.lineBufDirty) {
              convert_line();
            }

            if(save_clock_date() == 0) {
              scr_set(1);
            }
          }
          // F10? (discard changes)
          else if(memcmp(&gState.escapeBuf, &kEscF10, 3) == 0) {
            scr_set(1);
          }
          // regardless, back to idle
          gState.state = kStateIdle;
        }

        break;
      }

      // blank line 20 and go back to idle
      case kStateAfterError: {
        puts("\a\033[20;1H\033[K");
        gState.cursorDirty = true;

        gState.state = kStateIdle;
        break;
      }

      // unknown state, shouldn't get here
      default:
        break;
    }
  }

  // handle drawing
  if(gState.formValuesDirty) {
    draw_form_values();
  }
  if(gState.cursorDirty) {
    update_cursor();
  }
}

/**
 * "About to disappear" function for clock screen
 */
void scr_clock_disappear() {

}



/**
 * Sets the cursor's position in the form.
 */
static void update_cursor() {
  // set the appropriate position
  printf("\033[%d;10H", (gState.formRow + 5));
  gState.cursorDirty = false;

  // re-enable echo
  tty_set_echo(true);
}

/**
 * Draws the values in the form.
 */
static void draw_form_values() {
  tty_set_echo(false);

  // output the numeric values
  for(int i = 0; i < 6; i++) {
    printf("\033[%d;10H%2d", (i + 5), gState.values[i]);
  }

  // then print the day-of-week
  int day = get_day_of_week(gState.values[2], gState.values[1], gState.values[0]);
  printf("\033[7;13H (%s)    ", gDayNames[day & 0x07]);

  // form is no longer dirty
  gState.formValuesDirty = false;

  tty_set_echo(true);
}

/**
 * Actually sets the clock.
 */
static int save_clock_date() {
  int err;
  clock_t clock;

  // set up the values we want to write
  clock.century = 20;
  clock.year = gState.values[0];
  clock.month = gState.values[1];
  clock.day = gState.values[2];
  clock.dayOfWeek = get_day_of_week(clock.day, clock.month, clock.year);

  clock.hours = gState.values[3];
  clock.minutes = gState.values[4];
  clock.seconds = gState.values[5];
  clock.msecs = 0;

  // write clock
  err = clock_write(&clock);

  if(err != 0) {
    // print error message (and ensure to hide cursor)
    tty_set_echo(false);
    printf("\a\033[20;1H\033[7mFailed to write to RTC: %d\033[0m", err);

    return -1;
  }

  // success!
  return 0;
}


/**
 * Given a year, month, and day, calculate the day of the week.
 */
static int get_day_of_week(uint8_t d, uint8_t m, uint8_t y1) {
  int y = (y1 + 2000);

  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  y -= m < 3;
  return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

/**
 * Converts what has been read in of the current line to the line value.
 */
static void convert_line() {
  int value = strtol(gState.lineBuf, NULL, 0);

  if(gState.values[gState.formRow] != value) {
    gState.values[gState.formRow] = value & 0xff;
    gState.formValuesDirty = true;
  }

  gState.lineBufDirty = false;
}

/**
 * Pre-fills the line buffer with the current value of the line.
 */
static void prepare_line() {// update the line buffer state
  gState.lineBufOffset = 0;
  snprintf(gState.lineBuf, 4, "%02d", gState.values[gState.formRow]);

  gState.lineBufDirty = false;
}
