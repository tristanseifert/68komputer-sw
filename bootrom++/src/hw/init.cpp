#include <new.h>

#include "hw.h"
#include "xr68c681/Xr68c681.h"

#include "io/Console.h"
#include "io/UnitTable.h"
#include "vector/table.h"

using namespace hw;

/*
 * Buffers for holding hardware drivers for devices on this board. These are allocated into using
 * placement new, since we can't really rely on the order of static initializers.
 */
static uint8_t __attribute__((aligned(alignof(Xr68C681)))) gDuartBuffer[sizeof(Xr68C681)];

/**
 * Initializes all drivers in the boot ROM. This is called before the main function for the prompt
 * handling is invoked. All drivers for hardware on the CPU board for which this ROM is compiled
 * will be initialized.
 */
void hw_init() {
    // initialize various tables and subsystems
    Vectors::InitTrampolines();
    io::UnitTable::Init();

    Console::Init();

    // initialize DUART first and set it up as the console output
    auto duart = new (reinterpret_cast<Xr68C681 *>(gDuartBuffer)) Xr68C681(
            reinterpret_cast<volatile void *>(0x130001), 0xF0);
    Console::SetDevice(duart->getPortA());

    // reset terminal
    constexpr static const char kReset[]{
        // reset terminal attributes
        0x1b, '[', '0', 'm',
        // set ASCII character set for set 0
        0x1b, '(', 'B',
        // set DEC special graphics for set 1
        0x1b, ')', '0', 0
    };

    Console::Put(kReset);
}

