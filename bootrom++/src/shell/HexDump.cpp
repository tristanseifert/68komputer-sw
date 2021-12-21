#include "HexDump.h"

#include "io/Console.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace shellcmd;

/**
 * Processes the hexdump command.
 *
 * At least one argument is mandatory; we'll try to find a second argument, and both will be parsed
 * as integers.
 *
 * @remark Both values are always parsed as base 16.
 */
int HexDump::Handle(const char *, char *params) {
    // validate parameters
    if(!params) {
mcdonalds:;
        Console::Put("Invalid arguments!\r\n");
        PrintUsage();
        return -1;
    }

    // parse arg string to find start and length
    uintptr_t start{0};
    size_t length{128};

    const char *startPtr{params}, *lengthPtr{nullptr};

    startPtr = strtok(params, " ");
    lengthPtr = strtok(nullptr, " ");

    // ensure there's no excess args then parse them
    start = strtoul(startPtr, nullptr, 16);

    if(lengthPtr) {
        if(strchr(lengthPtr, ' ')) goto mcdonalds;

        length = strtoul(lengthPtr, nullptr, 16);
        if(!length) goto mcdonalds;
    }

    // align the addresses and lengths
    if(start & 0xF) {
        const auto diff = 0x10 - (start & 0xF);
        start -= (start & 0xF);
        length += diff;
    }

    if(length & 0xF) {
        length += 0x10 - (length & 0xF);
    }

    PrintDump(start, length);
    return 0;
}

/**
 * Performs a hex dump, starting at the given address and outputting the given number of bytes.
 *
 * @param start Physical start address, must be 16-byte aligned
 * @param length Number of bytes to output; must be a multiple of 16
 */
void HexDump::PrintDump(const uintptr_t start, const size_t length) {
    // output the header row
    Console::Put("\x1b[1;7m Address  | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF\x1b[0m\r\n");

    // process each row
    const size_t numRows = length / 16;
    auto addr = reinterpret_cast<const uint8_t *>(start);

    for(size_t row = 0; row < numRows; row++) {
        // write the current address
        Console::Print("$%8p | ", addr);

        // then the hex values
        char buf[4]{' ', ' ', ' ', '\0'};

        for(size_t x = 0; x < 16; x++) {
            const auto val = addr[x];

            buf[0] = ((val & 0xF0) >> 4) + '0';
            if(buf[0] > '9') buf[0] += 7;
            buf[1] = (val & 0x0F) + '0';
            if(buf[1] > '9') buf[1] += 7;

            if(x == 15) {
                buf[2] = '\0';
            }

            Console::Put(buf);
        }

        // divider
        Console::Put(" | ");

        // ASCII values
        for(size_t x = 0; x < 16; x++) {
            const unsigned char ch = addr[x];
            if(isprint(ch)) {
                Console::Put(ch);
            } else {
                Console::Put('.');
            }
        }

        // set up for next line
        Console::Put("\r\n");
        addr += 0x10;
    }
}

/**
 * Output a message indicating how to use the command.
 */
void HexDump::PrintUsage() {
    Console::Put("usage: hexdump [address] [length]\r\n"
            "  where length is optional (default 128)\r\n");
}
