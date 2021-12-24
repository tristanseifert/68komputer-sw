#pragma once

#include "Shell.h"

#include <stdint.h>

namespace shellcmd {
class Download {
    public:
        /// Return the descriptor for this command
        constexpr static inline Shell::Command GetCommandDescriptor() {
            return {
                .name = "download",
                .shortHelp = "Receive data over serial",
                .longHelp = "Accept binary data via XMODEM over a serial port. The data can be written\r\n"
                    "anywhere in memory, and any available serial port can be used to receive from.\r\n"
                    "See the \x1b[1minventory\x1b[0m command to show all ports.\r\n\n"
                    " - \x1b[1mdownload\x1b[0m [port]: Accept transfers on the given serial port.\r\n"
                    " - \x1b[1mdownload\x1b[0m [port] [addr]: Accept transfers and write them to a different memory\r\n"
                    "   location.\r\n\n"
                    "If no address is specified, it defaults to the start of RAM. Address must be\r\n"
                    "specified in hexadecimal.\r\n",
                .handler = Handle,
            };
        }


    private:
        static int Handle(const char *cmd, char *params);
};
}
