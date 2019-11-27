#include <stdio.h>

/**
 * Reads a line from the TTY.
 */
char* gets(char *out, size_t outLen) {
  size_t counter = 0;
  char *start = out;

  // keep reading chars
  int c = getchar() & 0xFF;

  while(c != 0xd && counter < outLen) {
    // printf("got char: $%08x\r\n", c);

    // store the character and increment counter
    *out++ = c;
    counter++;

    // read new character
    c = getchar() & 0xFF;
  }

  // zero-terminate the string
  *out = '\0';

  // return location of start of string
  return start;
}
