#include "RomApps.h"

#include "romapp_basic.h"

#include "io/Console.h"
#include "util/Compression.h"

#include <string.h>

using namespace shellcmd;

extern "C" {
extern void shell_exec_handler();
}

/**
 * Copies the BASIC interpreter to the top of RAM at $F00000, and jumps to it.
 */
int RomApps::HandleBasic(const char *, char *) {
    auto addr = reinterpret_cast<void *>(0xF00000);
    auto source = reinterpret_cast<const void *>(ehbasic_bin_lz4);

    // decompress the interpreter
    Console::Print("Decompressing %lu bytes at $%p to $%p... ", ehbasic_bin_lz4_len, source, addr);
    util::Compression::DecompressLz4(source, addr);
    Console::Put("done \r\n");

    // launch it
    auto hdr = reinterpret_cast<const AppHeader *>(addr);
    return LaunchApp(*hdr);
}

/**
 * Launches an app.
 */
int RomApps::LaunchApp(const AppHeader &hdr) {
    // validate header
    if(hdr.magic != 'APPE') {
        Console::Print("Invalid app %s ($%08lx)\r\n", "header magic", hdr.magic);
        return -1;
    }
    else if((hdr.version & 0xFF00) != 0x0100) {
        Console::Print("Invalid app header %s ($%04x)\r\n", "version", hdr.version);
        return -1;
    }
    else if(hdr.headerSz < sizeof(hdr)) {
        Console::Print("Invalid app header %s ($%04x)\r\n", "size", hdr.headerSz);
        return -1;
    }

    // ensure the length and entry point make sense
    else if(hdr.length < hdr.headerSz) {
        Console::Print("Invalid app %s ($%08lx)\r\n", "length", hdr.length);
        return -1;
    }
    else if(hdr.entry > hdr.length) {
        Console::Print("Invalid app %s ($%08lx)\r\n", "entry", hdr.entry);
        return -1;
    }

    // jump to its entry point
    auto entryAddr = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(&hdr) + hdr.entry);
    auto appName = hdr.name ? reinterpret_cast<const char *>(reinterpret_cast<uintptr_t>(&hdr) + hdr.name) : nullptr;

    Console::Print("Launching %s (at $%p)\r\n", appName ? appName : "app", entryAddr);

    asm volatile(R"(
    mov.l       %0, %%d0
    jmp         shell_exec_handler
    )" : /* no outputs */ : "g"(entryAddr) : "d0");

    return 0;
}
