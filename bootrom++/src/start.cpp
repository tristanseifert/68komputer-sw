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
 * Prints the sign-on banner.
 *
 * This is a box containing the monitor build info and reset reason.
 */
static void PrintBanner() {
    constexpr static const uint8_t kBannerWidth{50};
    Console::Put("\r\n \x0e\x6c");
    for(uint8_t i = 0; i < kBannerWidth-2; i++) {
        Console::Put(0x71);
    }
    Console::Put("\x6b\x0f\r\n");

    Console::Print(" \x0e\x78\x0f 68komputer monitor, version %-18s \x0e\x78\x0f\r\n", gVersionShort);
    Console::Print(" \x0e\x78\x0f Reset reason: %-32s \x0e\x78\x0f\r\n",
            (resetreason < kMaxResetReason) ? gResetReasonNames[resetreason] : "unknown");

    Console::Put(" \x0e\x6d");
    for(uint8_t i = 0; i < kBannerWidth-2; i++) {
        Console::Put(0x71);
    }
    Console::Put("\x6a\x0f\r\n");
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
    PrintBanner();

    // enter the shell thingie
    for(;;) {
        Shell::RunPrompt(true);
    }
}
