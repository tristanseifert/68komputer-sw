#include <new.h>

#include "hw.h"
#include "xr68c681/Xr68c681.h"

#include "io/Console.h"
#include "vector/table.h"

using namespace hw;

/*
 * Buffers for holding hardware drivers for devices on this board. These are allocated into using
 * placement new, since we can't really rely on the order of static initializers.
 */
static uint8_t __attribute__((aligned(alignof(Xr68C681)))) gDuartBuffer[sizeof(Xr68C681)];

/**
 * Initializes all drivers in the boot ROM. This is called before the main function for the prompt
 * handling is invoked.
 */
void hw_init() {
    // initialize interrupt handling and console
    Vectors::InitTrampolines();
    Console::Init();

    // initialize DUART first and set it up as the console output
    auto duart = new (reinterpret_cast<Xr68C681 *>(gDuartBuffer)) Xr68C681(
            reinterpret_cast<volatile void *>(0x130001), 0xF0);
    Console::SetDevice(duart->getPortA());
}

