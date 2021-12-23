#pragma once
#include "Shell.h"

#include <stdint.h>

namespace shellcmd {
/**
 * Produces an inventory of devices in the system.
 *
 * Currently, two separate commands are supported: `inventory` for a listing of hardware devices,
 * and `units` for a listing of drivers' registered devices.
 */
class Inventory {
    public:
        /// Command descriptor for the `units` command
        constexpr static inline Shell::Command GetUnitsCommandDescriptor() {
            return {
                .name = "units",
                .shortHelp = "Show registered device drivers",
                .longHelp = "Dump the contents of the unit table, which is used by firmware to look up device\r\n"
                    "drivers to handle IO.\r\n\n"
                    "See also: \x1b[1minventory\x1b[0m\r\n\n",
                .handler = HandleUnits,
            };
        }

    private:
        static int HandleUnits(const char *cmd, char *params);
};
}
