#include "io/Console.h"

extern "C" void bootrom_start();

/**
 * Entry point for the boot ROM
 *
 * This is where we'll check the hardware configuration in NVRAM and the DIP switches to figure
 * out whether we should look for an application to boot, or drop into the boot ROM monitor.
 */
void bootrom_start() {
    Console::Puts("Yo what's up\r\n");
    Console::Puts("Today were going to smoke weed\r\n");

    // XXX: infinite loop
    for(;;) {}
}
