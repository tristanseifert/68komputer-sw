#pragma once

#include "Shell.h"

#include <stdint.h>

namespace shellcmd {
/**
 * Handles launching ROM apps.
 */
class RomApps {
    public:
        /// Command descriptor for launching the BASIC interpreter
        constexpr static inline Shell::Command GetBasicCommandDescriptor() {
            return {
                .name = "basic",
                .shortHelp = "Launch BASIC interpreter",
                .longHelp = "Decompresses Lee Davison's Enhanced 68k Basic into RAM, and launches it.\r\n",
                .handler = HandleBasic,
            };
        }

    private:
        static int HandleBasic(const char *cmd, char *params);
};
}
