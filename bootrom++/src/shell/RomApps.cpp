#include "RomApps.h"

#include "romapp_basic.h"

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

    // copy the interpreter
    memcpy(addr, ehbasic_bin, ehbasic_bin_len);

    // jump to it (never returns)
    asm volatile(R"(
    mov.l       %0, %%d0
    jmp         shell_exec_handler
    )" : /* no outputs */ : "g"(addr) : "d0");

    return 0;
}

