#include "Execute.h"
#include "Register.h"
#include "io/Console.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace shellcmd;

extern "C" {
// helper methods defined in Execute.asm; it uses a custom register calling convention
extern void shell_exec_handler();
// Prepare the machine and jump back to the ROM entry point
extern void shell_exec_reset();

// clear to 0 to force a cold boot
extern uint16_t bootflag;
}

/**
 * Jumps to the user code loaded at the specified address.
 */
int Execute::HandleExec(const char *command, char *params) {
    char *addrStr;

    if(!params || !(addrStr = strtok(params, " "))) {
        Console::Print("%s: %s\r\n", command, "invalid parameters");
        return -1;
    }


    // do a nasty hack to simulate a regparam function call
    const auto addr = reinterpret_cast<volatile void *>(strtoul(addrStr, nullptr, 16));

    asm volatile(R"(
    mov.l       %0, %%d0
    jmp         shell_exec_handler
    )" : /* no outputs */ : "g"(addr) : "d0");

    // shut up compiler; we return under error conditions so it can't be noreturn
    return 0;
}

/**
 * Simulates a hardware reset. If a parameter is specified, it should be the word `cold` and we'll
 * set up as if we are doing a cold boot instead.
 */
int Execute::HandleReset(const char *command, char *params) {
    bool cold{false};

    // check if we've got a paramy
    if(params) {
        const auto first = strtok(params, " ");
        if(first && strncmp(first, "cold", 4) == 0) {
            cold = true;
        }
        // params specified but we failed to parse out the string
        else {
            Console::Print("%s: %s\r\n", command, "invalid parameters");
            return -1;
        }
    }

    // do reset
    if(cold) {
        bootflag = 0;
    }

    shell_exec_reset();

    // shut up compiler; we return under error conditions so it can't be noreturn
    return 0;
}

