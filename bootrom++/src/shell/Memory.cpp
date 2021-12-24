#include "Memory.h"
#include "io/Console.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace shellcmd;

/**
 * Perform a memory read from the given address.
 *
 * We expect two arguments, first the size, then the hexadecimal address.
 */
int Memory::HandleRead(const char *command, char *params) {
    void *addr;
    Size size;

    if(!params) {
        Console::Print("%s: %s\r\n", command, "parameters required");
        return -1;
    }

    // parse args
    const auto sizeArg = strtok(params, " ");
    const auto addrArg = strtok(nullptr, " ");

    if(!sizeArg || !addrArg) {
        Console::Print("%s: %s\r\n", command, "invalid parameters");
        return -1;
    }

    size = ParseSize(sizeArg);
    if(size == Size::Invalid) {
        Console::Print("%s: %s\r\n", command, "invalid size");
        return -1;
    }

    addr = reinterpret_cast<void *>(strtoul(addrArg, nullptr, 16));

    // refuse an access on an odd address (XXX: disableable?)
    if(size != Size::Byte && reinterpret_cast<uintptr_t>(addr) & 0b01) {
        Console::Print("%s: %s\r\n", command, "invalid address");
        return -1;
    }

    // perform access
    switch(size) {
        case Size::Byte: {
            uint8_t temp;
            memcpy(&temp, addr, sizeof(temp));
            Console::Print("$%p = $%02X\r\n", addr, temp);
            break;
        }
        case Size::Word: {
            uint16_t temp;
            memcpy(&temp, addr, sizeof(temp));
            Console::Print("$%p = $%04X\r\n", addr, temp);
            break;
        }
        case Size::Longword: {
            uint32_t temp;
            memcpy(&temp, addr, sizeof(temp));
            Console::Print("$%p = $%08lX\r\n", addr, temp);
            break;
        }

        default:
            return -1;
    }

    return 0;
}

/**
 * Perform a memory write to the given address.
 *
 * We expect three arguments, first the size, then the hexadecimal address and data.
 */
int Memory::HandleWrite(const char *command, char *params) {
    void *addr;
    uint32_t data;
    Size size;

    if(!params) {
        Console::Print("%s: %s\r\n", command, "parameters required");
        return -1;
    }

    // parse args
    const auto sizeArg = strtok(params, " ");
    const auto addrArg = strtok(nullptr, " "), dataArg = strtok(nullptr, " ");

    if(!sizeArg || !addrArg || !addrArg) {
        Console::Print("%s: %s\r\n", command, "invalid parameters");
        return -1;
    }

    size = ParseSize(sizeArg);
    if(size == Size::Invalid) {
        Console::Print("%s: %s\r\n", command, "invalid size");
        return -1;
    }

    addr = reinterpret_cast<void *>(strtoul(addrArg, nullptr, 16));
    data = strtoul(dataArg, nullptr, 16);

    // refuse an access on an odd address (XXX: disableable?)
    if(size != Size::Byte && reinterpret_cast<uintptr_t>(addr) & 0b01) {
        Console::Print("%s: %s\r\n", command, "invalid address");
        return -1;
    }

    // perform access
    switch(size) {
        case Size::Byte: {
            const uint8_t temp = data;
            memcpy(addr, &temp, sizeof(temp));
            Console::Print("$%p = $%02X\r\n", addr, temp);
            break;
        }
        case Size::Word: {
            const uint16_t temp = data;
            memcpy(addr, &temp, sizeof(temp));
            Console::Print("$%p = $%04X\r\n", addr, temp);
            break;
        }
        case Size::Longword: {
            const uint32_t temp = data;
            memcpy(addr, &temp, sizeof(temp));
            Console::Print("$%p = $%08lX\r\n", addr, temp);
            break;
        }

        default:
            return -1;
    }

    return 0;
}


/**
 * Parse the value of the size parameter to a memory command.
 */
Memory::Size Memory::ParseSize(const char *sizeStr) {
    switch(sizeStr[0]) {
        case '1':
        case 'b':
        case 'B':
            return Size::Byte;

        case '2':
        case 'w':
        case 'W':
            return Size::Word;

        case '4':
        case 'l':
        case 'L':
            return Size::Longword;

        default:
            return Size::Invalid;
    }
}

