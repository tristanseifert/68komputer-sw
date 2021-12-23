#pragma once

#include <stddef.h>
#include <stdint.h>

struct driver_info_t;

namespace io {
/**
 * Table holding all initialized device objects
 *
 * When a driver is initialized for discovered hardware, any devices it exposes will be registered
 * in the unit table.
 */
class UnitTable {
    public:
        /**
         * Device classes that can be registered in the unit table.
         */
        enum class Type: uint8_t {
            /// Device that doesn't fit into any of the other categories
            Unknown                     = 0,
            /// Character device (serial port)
            CharSerial                  = 1,
            /// Character device (other)
            Char                        = 2,
            /// Block device (floppy or hard drive)
            BlockDisk                   = 3,

            /// Maximum value of the type enum
            MaxType,

            // Invalid type (used to mark an unit that's available for allocation)
            Invalid                     = 0xFF,
        };

        /**
         * Address of a device driver; this consists of an unit type and unit number.
         */
        struct Address {
            Type type{Type::Unknown};
            uint8_t index{0};
        };

        /**
         * Defines a single device (unit) registered with the IO subsystem. Each unit is assigned
         * a particular type, and an unique index in that type's namespace.
         */
        struct Unit {
            Type type;
            uint8_t index{0};

            /// Driver that registered this unit
            const driver_info_t *driver{nullptr};
            /// Payload data; its interpretation is up to the unit
            void *payload{nullptr};

            /// Compares the unit to an unit address
            constexpr inline bool operator==(const Address &addr) const {
                return (addr.type == type) && (addr.index == index);
            }
        };

    public:
        static void Init();

        static int Register(const Type type, const driver_info_t &driver, void *payload);

        /**
         * Converts the given unit name to an address, then attempts to look up the unit.
         */
        static void *GetUnit(const char *name) {
            Address addr;
            if(!ParseUnitName(name, addr)) return nullptr;
            return GetUnit(addr);
        }
        static void *GetUnit(const Address &addr);

        static bool ParseUnitName(const char *name, Address &outAddress);
        /**
         * Generate a name string for the given unit's address.
         *
         * @param name Buffer to receive the name
         * @param nameLen Length of the name buffer
         * @param unit Unit to generate the name string for
         */
        static void NameForUnit(char *name, const size_t nameLen, const Unit &unit) {
            Address a{unit.type, unit.index};
            NameForUnit(name, nameLen, a);
        }
        static void NameForUnit(char *name, const size_t nameLen, const Address &address);

        static void IterateTable(bool (*callback)(const Unit &unit, size_t i, void *ctx), void *ctx);

    public:
        /**
         * Total number of units to reserve space for.
         *
         * The number of units reserved affects how many peripheral devices can be used in the
         * system. When the unit table is full, any additional detected devices can't be used by
         * IO routines exposed by the ROM. However, client applications _can_ directly access
         * the hardware in this case.
         */
        static const constexpr size_t kNumUnits{32};

    private:
        /**
         * Gets an unit id to allocate to a device of the given type.
         */
        static inline uint8_t NextId(const Type type) {
            return gUnitCount[static_cast<uint8_t>(type)]++;
        }

    private:
        /**
         * Unit table storage. Entries are allocated sequentially, into the first available slot
         * found.
         */
        static Unit gUnits[kNumUnits];

        /**
         * A mapping of type ids -> type prefix. These are used to determine what unit type to
         * search for given a device name such as `TTY0` for example.
         *
         * This array is used as a map; the index corresponds to the corresponding type enum value.
         */
        static const char * const gTypePrefix[static_cast<size_t>(Type::MaxType)];

        /**
         * Holds the index value for the next unit of a particular type.
         */
        static uint8_t gUnitCount[static_cast<size_t>(Type::MaxType)];
};
}
