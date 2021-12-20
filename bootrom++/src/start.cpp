#include "io/Console.h"

#include <stdint.h>

#include <printf.h>

extern "C" void bootrom_start();

/**
 * Entry point for the boot ROM
 *
 * This is where we'll check the hardware configuration in NVRAM and the DIP switches to figure
 * out whether we should look for an application to boot, or drop into the boot ROM monitor.
 */
void bootrom_start() {
    Console::Put("Yo what's up, today were going to smoke weed\r\n");

    Console::Print("Kush number = %d\r\n", 420);

    // XXX: infinite loop
    for(;;) {}
}
