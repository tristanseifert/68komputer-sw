#include "io/Console.h"
#include "shell/Shell.h"

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
    // initialize some stuff
    Shell::Init();

    // TODO: check NVRAM, hardware, DIP switches

    // enter the shell thingie
    for(;;) {
        Shell::RunPrompt(true);
    }
}
