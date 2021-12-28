#pragma once

#include "Shell.h"

#include "hw/Registers.h"

#include <string.h>

namespace shellcmd {
/**
 * Inspect the contents of registers
 */
class Register {
    public:
        /// Command descriptor for resetting the system
        constexpr static inline Shell::Command GetCommandDescriptor() {
            return {
                .name = "reg",
                .shortHelp = "Inspect register state",
                .longHelp = "Read and modify the contents of a program's registers. This works in\r\n"
                    "conjunction with other debugging commands.\r\n"
                    "\n"
                    " - \x1b[1mreg\x1b[0m: Dump the current state of all registers\r\n"
                    " - \x1b[1mreg\x1b[0m [register] [value]: Change the value of a register\r\n\n",
                .handler = Handle,
            };
        }

        /// Clear the app return registers
        static inline void ClearReturnRegs() {
            memset(&gReturnRegs, 0, sizeof(gReturnRegs));
        }

        /// Clear the exception registers
        static inline void ClearExceptionRegs() {
            memset(&gExceptionRegs, 0, sizeof(gExceptionRegs));
        }

        static void DumpRegisters(const CpuRegisters &regs);

    private:
        static int Handle(const char *cmd, char *params);

    private:
        // registers stored when a program exits
        static CpuRegisters gReturnRegs;
        // registers stored when an exception is taken
        static CpuRegisters gExceptionRegs;
};
}

