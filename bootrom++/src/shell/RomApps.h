#pragma once

#include "Shell.h"

#include <stdint.h>

namespace shellcmd {
/**
 * Handles launching ROM apps.
 */
class RomApps {
    private:
        /**
         * Stores information about an app.
         *
         * All offsets/pointers are relative to the start of this header.
         */
        struct AppHeader {
            // should be 'APPE'
            uint32_t magic;
            // version of header (should be 0x100)
            uint16_t version;
            // length of header, in bytes
            uint16_t headerSz;
            // length of the app, in bytes (after header)
            uint32_t length;

            // app name offset
            uint32_t name;
            // entry point offset
            uint32_t entry;
        } __attribute__((packed));

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

        static int LaunchApp(const AppHeader &);
};
}
