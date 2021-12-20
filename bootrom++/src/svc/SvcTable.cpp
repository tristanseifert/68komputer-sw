#include "SvcTable.h"

#include "io/Console.h"

#include <printf.h>

/**
 * Define the services exposed by the ROM. This is placed at the start of the ROM, right after the
 * vector table.
 *
 * Its location can be read from vector $FF in the vector table.
 */
const __attribute__((section(".svctable"))) SvcTable SvcTable::gTable = {
    .magic = 'FUNC',
    .version = 0x00001000,

    // console
    .ConsPuts = Console::Put,
    .ConsPrint = Console::Print,

    // string
    .StrFormatBufferList = vsnprintf,
    .StrFormatBuffer = snprintf,
};
