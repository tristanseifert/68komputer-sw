#include "Download.h"
#include "io/Console.h"
#include "io/XModemReceiver.h"
#include "io/UnitTable.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace shellcmd;

/**
 * Process the `download` command.
 *
 * At least one argument must be specified, which is the port name to receive from. If a second
 * argument is specified, it will be the download address.
 *
 * Downloads use the XMODEM protocol, with CRC instead of a traditional checksum preferred.
 */
int Download::Handle(const char *command, char *params) {
    using namespace io;

    const char *portName, *addrString{nullptr};
    uintptr_t addr{0xF00000}; // XXX: can we not hardcode this?

    // parse arguments
    if(!params) {
        Console::Print("%s: %s\r\n", command, "invalid parameters");
        return -1;
    }

    portName = strtok(params, " ");
    addrString = strtok(nullptr, " ");

    if(addrString) {
        addr = strtoul(addrString, nullptr, 16);
    }

    // set up the port
    UnitTable::Address portAddr;
    if(!UnitTable::ParseUnitName(portName, portAddr)) {
        Console::Print("%s: %s\r\n", command, "invalid unit name");
        return -1;
    }
    // TODO: validate type of port?

    auto rawPort = io::UnitTable::GetUnit(portAddr);
    if(!rawPort) {
        Console::Print("%s: %s\r\n", command, "failed to look up unit");
        return -1;
    }

    auto port = reinterpret_cast<io::CharacterDevice *>(rawPort);

    // drain receive queue on the port
    constexpr static const size_t kDrainLimit{128};
    size_t drained{0};

    while(port->readAvailable() && drained++ < kDrainLimit) {
        (void) port->read();
    }
    if(drained >= kDrainLimit) {
        Console::Print("Failed to drain port receive buffer (read %lu bytes)\r\n", drained);
        return -1;
    }

    // do download
    XModemReceiver xr(port);

    Console::Print("Ready to accept XMODEM transfers.\r\nPort: %-10s ($%p)    Address: $%08lX\r\n",
            portName, port, addr);

    const auto ret = xr.receive(reinterpret_cast<void *>(addr), true);
    if(ret < 0) {
        Console::Print("Receive failed: %d\r\n", ret);
    } else {
        Console::Print("Received %d bytes\r\n", ret);
        return 0;
    }

    return ret;
}
