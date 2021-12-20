#include "hw.h"
#include "Xr68c681.h"

#include "io/Console.h"
#include "vector/table.h"

using namespace hw;

/**
 * Initializes all drivers in the boot ROM. This is called before the main function for the prompt
 * handling is invoked.
 */
void hw_init() {
    // initialize interrupt handling and console
    Vectors::InitTrampolines();
    Console::Init();

    // then, initialize hardware
    Xr68C681::Reset();
}

