#include "io/Console.h"
#include "shell/Shell.h"
#include "runtime/version.h"


#include <stdint.h>

#include <printf.h>

extern "C" {
void bootrom_start();

// 0 = cold boot, 1 = warm reset
extern uint8_t resetreason;
}

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
    Console::Print("68komputer monitor (version %s)\r\n", gVersionShort);
    Console::Print("Reset reason: %s\r\n", (resetreason == 0) ? "cold boot" : "warm reset");

    // enter the shell thingie
    for(;;) {
        Shell::RunPrompt(true);
    }
}
