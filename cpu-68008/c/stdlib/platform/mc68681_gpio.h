#ifndef PLATFORM_MC68681_GPIO
#define PLATFORM_MC68681_GPIO

#include <stdint.h>

/**
 * Returns the state of the 68681 GPIOs.
 */
int gpio_read(void);

/**
 * Sets the state of the 68681 GPIOs.
 */
int gpio_set(uint8_t state);

#endif
