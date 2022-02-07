#pragma once

#include "Shell.h"

namespace shellcmd {
/**
 * Simple memory modification commands (reading and writing)
 *
 * Specifically, the handled commands are:
 *
 * -  Reads: `mr [size] [addr]`
 * - Writes: `mw [size] [addr] [value]`
 *
 * Size may be one of 1/2/4 or b/w/l respectively. Address and value are specified in hexadecimal.
 */
class Memory {
    public:
        /// Command descriptor for memory read
        constexpr static inline Shell::Command GetReadCommandDescriptor() {
            return {
                .name = "mr",
                .shortHelp = "Peek at memory",
                .longHelp = "Perform a read from memory.\r\n\n"
                    " - \x1b[1mmr\x1b[0m [size] [address]: Read from a single memory address.\r\n\n"
                    "Size can be one of {b,w,l} or {1,2,4}. Address is specified in hexadecimal.\r\n",
                .handler = HandleRead,
            };
        }

        /// Command descriptor for memory write
        constexpr static inline Shell::Command GetWriteCommandDescriptor() {
            return {
                .name = "mw",
                .shortHelp = "Poke at memory",
                .longHelp = "Write data to an arbitrary memory location.\r\n\n"
                    " - \x1b[1mmb\x1b[0m [size] [address] [data]: Write to a single memory address.\r\n\n"
                    "Size can be one of {b,w,l} or {1,2,4}. Address and data are specified in\r\n"
                    "hexadecimal.\r\n",
                .handler = HandleWrite,
            };
        }

    private:
        enum class Size: uint8_t {
            Invalid                     = 0,
            Byte                        = 1,
            Word                        = 2,
            Longword                    = 4,
        };

        static int HandleRead(const char *cmd, char *params);
        static int HandleWrite(const char *cmd, char *params);

        static Size ParseSize(const char *sizeStr);
};
}
