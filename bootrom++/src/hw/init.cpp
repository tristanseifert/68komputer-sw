#include "hw.h"
#include "Xr68c681.h"

using namespace hw;

/**
 * Initializes all drivers in the boot ROM. This is called before the main function for the prompt
 * handling is invoked.
 */
void hw_init() {
    Xr68C681::Reset();
}

