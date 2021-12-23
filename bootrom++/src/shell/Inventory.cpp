#include "Inventory.h"
#include "io/Console.h"
#include "io/UnitTable.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <bootrom/Driver.h>

using namespace shellcmd;

/**
 * Prints the contents of the unit table.
 */
int Inventory::HandleUnits(const char *, char *) {
    using namespace io;

    Console::Print("Unit table (max size %lu):\r\n", UnitTable::kNumUnits);
    Console::Print("\x1b[1;7m #  %8s Address   %-16s\x1b[0m\r\n", "Name", "Driver Name");

    UnitTable::IterateTable([](auto unit, auto i, auto) -> bool {
        constexpr static const size_t kNameBufLen{8};
        char name[kNameBufLen];

        UnitTable::NameForUnit(name, kNameBufLen, unit);

        Console::Print("%2lu. %8s $%p %-16s\r\n", i+1, name, unit.payload, unit.driver->name);
        return true;
    }, nullptr);

    return 0;
}

