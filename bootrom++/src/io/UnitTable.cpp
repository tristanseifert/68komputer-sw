#include "UnitTable.h"

#include "Console.h"

#include <ctype.h>
#include <printf.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

using namespace io;

UnitTable::Unit UnitTable::gUnits[kNumUnits];

const char * const UnitTable::gTypePrefix[static_cast<size_t>(Type::MaxType)] = {
    // Type::Unknown
    "UNK",
    // Type::CharSerial
    "TTY",
    // Type::Char
    "CHR",
    // Type::BlockDisk
    "BLK",
};

uint8_t UnitTable::gUnitCount[static_cast<size_t>(Type::MaxType)];



/**
 * Initialize the unit table. This will clear any existing reservations.
 */
void UnitTable::Init() {
    for(size_t i = 0; i < static_cast<size_t>(Type::MaxType); i++) {
        gUnitCount[i] = 0;
    }

    for(size_t i = 0; i < kNumUnits; i++) {
        auto &unit = gUnits[i];
        unit.type = Type::Invalid;
        unit.index = 0;
        unit.driver = nullptr;
        unit.payload = nullptr;
    }
}

/**
 * Adds a device of a given type to the unit table.
 *
 * @param type Type of the device to be registered
 * @param driver Driver info struct of whoever owns this device
 * @param payload The type specific payload needed to access this unit. May not be `nullptr`
 *
 * @return An unit id [0, 255] or a negative error code
 */
int UnitTable::Register(const Type type, const driver_info_t &driver, void *payload) {
    if(static_cast<uint8_t>(type) >= static_cast<uint8_t>(Type::MaxType) || !payload) {
        return -1;
    }

    for(size_t i = 0; i < kNumUnits; i++) {
        auto &unit = gUnits[i];
        if(unit.type == Type::Invalid) {
            unit.index = NextId(type);
            unit.driver = &driver;
            unit.payload = payload;
            unit.type = type;

            return unit.index;
        }
    }

    // no slots free in the unit table
    return -2;
}

/**
 * Looks up an unit in the unit table with the given (type, id) pair.
 *
 * @param addr Unit address to find
 *
 * @return The unit's payload, or `nullptr` if not found.
 */
void *UnitTable::GetUnit(const Address &addr) {
    for(size_t i = 0; i < kNumUnits; i++) {
        const auto &unit = gUnits[i];
        if(unit == addr) {
            return unit.payload;
        }
    }

    return nullptr;
}

/**
 * Parses the unit name out into a type and index. Unit names are of the form of `TYPEn``, where
 * `TYPE` is one of the values specified in gTypePrefix.
 *
 * @param name Full unit name string
 * @param outAddress If the string is valid, the address of the unit
 *
 * @return Whether the name string could be converted to an unit address
 */
bool UnitTable::ParseUnitName(const char *name, Address &outAddress) {
    char typeStr[8]{0}, indexStr[4]{0};
    uint8_t strOff{0}, typeLen{0};

    // extract the unit type and index strings
    while(true) {
        const char ch = *name;
        // if string ends already, it's not valid
        if(ch == '\0') {
            return false;
        }
        // if it's a digit, we've finished reading in the type string
        else if(isdigit(ch)) {
            typeLen = strOff;
            typeStr[strOff] = '\0';
            break;
        }
        // otherwise copy it as is
        else {
            if(strOff == 7) return false;
            typeStr[strOff++] = toupper(ch);
            name++;
        }
    }

    strOff = 0;

    while(true) {
        const char ch = *name++;
        // reached end of string
        if(ch == '\0') {
            indexStr[strOff] = '\0';
            break;
        }
        // copy as is
        else {
            if(strOff == 3) return false;
            indexStr[strOff++] = ch;
        }
    }

    // find type and convert index
    for(uint8_t i = 0; i < static_cast<uint8_t>(Type::MaxType); i++) {
        if(!strncmp(typeStr, gTypePrefix[i], typeLen)) {
            outAddress.type = static_cast<Type>(i);
            outAddress.index = strtoul(indexStr, nullptr, 10);
            return true;
        }
    }

    // if we get here, the type couldn't be found
    return false;
}

/**
 * Generate a name string for an unit at the given address.
 *
 * @param name Buffer to receive the name
 * @param nameLen Length of the name buffer
 * @param address Address to write a name string for
 */
void UnitTable::NameForUnit(char *name, const size_t nameLen, const Address &address) {
    if(address.type >= Type::MaxType) return;

    snprintf(name, nameLen, "%s%u", gTypePrefix[static_cast<size_t>(address.type)], address.index);
}

/**
 * Iterate over all occupied slots in the unit table.
 *
 * @param callback Function to invoke for each unit. Its arguments are:
 *   @param unit Unit object
 *   @param i Index into the unit table
 *   @param ctx Optional paramater/arguments passed in
 * @param ctx Parameter to pass to the callback
 *
 * @remark Return `false` from the callback at any time to abort iteration before the end is
 *         reached.
 */
void UnitTable::IterateTable(bool (*callback)(const Unit &unit, size_t i, void *ctx), void *ctx) {
    for(size_t i = 0; i < kNumUnits; i++) {
        const auto &unit = gUnits[i];
        if(unit.type == Type::Invalid) continue;

        if(!callback(unit, i, ctx)) return;
    }
}

