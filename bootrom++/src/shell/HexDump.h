#pragma once

#include "Shell.h"

#include <stdint.h>

namespace shellcmd {
/**
 * Implements the `hexdump` command which allows inspection of memory by dumping it to the console
 * like the `xxd` command line tool. The command can be invoked in one of the two following
 * forms:
 *
 * - hexdump [address]
 * - hexdump [address] [bytes]
 *
 *If the bytes count is not specified (first variant) it defaults to 128 bytes.
 */
class HexDump {
    public:
        /// Return the descriptor for this command
        constexpr static inline Shell::Command GetCommandDescriptor() {
            return {
                .name = "hexdump",
                .shortHelp = "Print contents of memory",
                .handler = Handle,
            };
        }


    private:
        static int Handle(const char *cmd, char *params);

        static void PrintDump(const uintptr_t start, const size_t length);

        static void PrintUsage();
};
}
