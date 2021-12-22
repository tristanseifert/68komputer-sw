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
     * Help: Prints all commands known to the shell out on the terminal. If an argument is
     * specified, we assume that's the name of a command to print detailed help for.
     */
    {
        .name = "help",
        .shortHelp = "How the hell does this thing work?",
        .longHelp = "Print help provided for built in commands:\r\n\n"
            " - \x1b[1mhelp\x1b[0m: Show a short overview of all available commands\r\n"
            " - \x1b[1mhelp\x1b[0m [command name]: Show detailed information specific to this command\r\n",
        .handler = Shell::HelpCommandHandler,
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

