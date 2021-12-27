#include "io/Console.h"
#include "shell/Shell.h"
#include "runtime/version.h"

#include <stddef.h>
#include <stdint.h>

#include <printf.h>

extern "C" {
void bootrom_start();

// 0 = cold boot, 1 = warm reset
volatile __attribute__((section(".bssnoclr"))) uint8_t resetreason{0};
// written to 'YE' at boot up to differentiate cold/warm reset
volatile __attribute__((section(".bssnoclr"))) uint16_t bootflag{0};
}

/// Maximum value for reset reasons, plus 1
static const constexpr size_t kMaxResetReason{3};

/**
 * A mapping of reset reason to a string describing it. This is used to print the reason during
 * boot up.
 */
static const char *const gResetReasonNames[kMaxResetReason] = {
    "cold boot",
    "warm reset",
    "application returned",
};

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
    Console::Print("Reset reason: %s\r\n",
            (resetreason < kMaxResetReason) ? gResetReasonNames[resetreason] : "unknown");

    // enter the shell thingie
    for(;;) {
        Shell::RunPrompt(true);
    }
}
