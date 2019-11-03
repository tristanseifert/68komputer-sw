/**
 * This prints out the input state of the 68681, and allows the user to input
 * the state of the outputs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include <platform/mc68681_gpio.h>

int main() {
  bool run = true;
  int ret;

  while(run) {
    // read GPIOs
    int read = gpio_read();
    printf("Input state: $%08x\r\n", read);

    // what does the user want to set GPIO to?
    int newState = 0;
    char buffer[16];

    puts("Output state? ");
    gets(buffer, sizeof(buffer));
    newState = strtol(buffer, NULL, 0);

    ret = gpio_set(newState);
    printf("\rOutput state: $%02x (return %i)\r\n\n", newState, ret);
  }

  return 0;
}
