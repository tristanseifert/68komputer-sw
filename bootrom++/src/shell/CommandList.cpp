#include "Shell.h"

#include "HexDump.h"

#include "runtime/version.h"
#include "io/Console.h"

using namespace shellcmd;

/*
 * List of all commands known by the boot ROM's little shell.
 *
 * @remark This list should be kept alphabetically ordered.
 */
const Shell::Command Shell::gCommands[kNumCommands] = {
    /**
     * Help: Prints all commands known to the shell out on the terminal.
     */
    {
        .name = "help",
        .shortHelp = "Show all known commands",
        .handler = [](auto, auto) -> int {
            Console::Put("***** Available Commands *****\r\n");
            for(size_t i = 0; i < kNumCommands; i++) {
                const auto &h = gCommands[i];
                Console::Print("%12s: %s\r\n", h.name, h.shortHelp ? h.shortHelp : "");
            }
            return 0;
        }
    },
    //  `hexdump`: read memory
    HexDump::GetCommandDescriptor(),
    /**
     * Version: Prints information about what version of the boot ROM is currently running,
     * including compile date.
     */
    {
        .name = "version",
        .shortHelp = "Show ROM version info",
        .handler = [](auto, auto) -> int {
            Console::Print("68komputer Boot ROM version %s (build %s)\r\n", gVersionShort,
                    gVersionCommit);
            Console::Put("Copyright 2019-2021: Tristan Seifert\r\n\n");
            Console::Put("Compiled on: " __DATE__ " at " __TIME__ "\r\n");
            return 0;
        }
    },
};

