#include "Register.h"
#include "io/Console.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace shellcmd;

CpuRegisters __attribute__((section(".bssnoclr"))) Register::gReturnRegs;
CpuRegisters __attribute__((section(".bssnoclr"))) Register::gExceptionRegs;

/**
 * Handle the register command; it will either print a register dump, or change one register's
 * value.
 */
int Register::Handle(const char *command, char *params) {
    const char *regNameStr, *regValStr;

    // no args = dump register state
    if(!params) {
        DumpRegisters(gReturnRegs);
        return 0;
    }
    // try to get arguments
    else if((regNameStr = strtok(params, " ")) && (regValStr = strtok(nullptr, ""))) {
        const auto typeChar = toupper(regNameStr[0]);

        // ensure name is valid
        if(strlen(regNameStr) > 2) {
            Console::Print("%s: %s\r\n", command, "invalid register specification");
            return -1;
        }
        else if(typeChar != 'A' && typeChar != 'D') {
            Console::Print("%s: %s\r\n", command, "invalid register type");
            return -1;
        }
        else if(regNameStr[1] < '0' || regNameStr[1] > '7') {
            Console::Print("%s: %s\r\n", command, "invalid register number");
            return -1;
        }

        // write its value
        const uint8_t regNum = regNameStr[1] - '0';
        const uint32_t value = strtoul(regValStr, nullptr, 16);

        if(typeChar == 'A') {
            gReturnRegs.a[regNum] = value;
        } else if(typeChar == 'D') {
            gReturnRegs.d[regNum] = value;
        }

        return 0;
    }

    // if we get here, failed to parse arguments
    Console::Print("%s: %s\r\n", command, "invalid parameters");
    return -1;
}

/**
 * Print out the contents of the register structure specified to the console.
 *
 * @param regs Register state to print
 */
void Register::DumpRegisters(const CpuRegisters &regs) {
    Console::Print("D0 $%08lX D1 $%08lX D2 $%08lX D3 $%08lX\r\n", regs.d[0], regs.d[1],
            regs.d[2], regs.d[3]);
    Console::Print("D4 $%08lX D5 $%08lX D6 $%08lX D7 $%08lX\r\n", regs.d[4], regs.d[5],
            regs.d[6], regs.d[7]);

    Console::Print("A0 $%08lX A1 $%08lX A2 $%08lX A3 $%08lX\r\n", regs.a[0], regs.a[1],
            regs.a[2], regs.a[3]);
    Console::Print("A4 $%08lX A5 $%08lX A6 $%08lX A7 $%08lX\r\n", regs.a[4], regs.a[5],
            regs.a[6], regs.a[7]);

    Console::Print("PC $%08lX SR $%04X\r\n", regs.pc, regs.sr);
}

