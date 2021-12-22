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
                .longHelp = "Output the contents of a region of memory to the console.\r\n\n"
                    " - \x1b[1mhexdump\x1b[0m [loc]: Output 128 bytes starting at `loc`.\r\n"
                    " - \x1b[1mhexdump\x1b[0m [loc] [n]: Output `n` bytes, starting at `loc`.\r\n\n"
                    "Both `loc` and `n` are specified in hexadecimal, with or without leading 0x.\r\n",
                .handler = Handle,
            };
        }


    private:
        static int Handle(const char *cmd, char *params);

        static void PrintDump(const uintptr_t start, const size_t length);

        static void PrintUsage();
};
}
