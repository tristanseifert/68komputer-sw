#pragma once

#include "Shell.h"

namespace shellcmd {
class Execute {
    public:
        /// Command descriptor for returning to a program
        constexpr static inline Shell::Command GetExecCommandDescriptor() {
            return {
                .name = "exec",
                .shortHelp = "Execute a program",
                .longHelp = "Set up a stack frame to return to the code at the given address.\r\n\n"
                    " - \x1b[1mexec\x1b[0m [address]: Begin executing code at address\r\n\n"
                    "Address is specified in hexadecimal. The command will set up a clean stack frame.\r\n",
                .handler = HandleExec,
            };
        }

        /// Command descriptor for resetting the system
        constexpr static inline Shell::Command GetResetCommandDescriptor() {
            return {
                .name = "reset",
                .shortHelp = "Reset the system",
                .longHelp = "Perform a warm reset, or a simulated cold boot.\r\n\n"
                    " - \x1b[1mreset\x1b[0m: Warm reset\r\n"
                    " - \x1b[1mreset\x1b[0m cold: Simulate a cold boot\r\n\n",
                .handler = HandleReset,
            };
        }

    private:
        static int HandleExec(const char *cmd, char *params);
        static int HandleReset(const char *cmd, char *params);
};
}
