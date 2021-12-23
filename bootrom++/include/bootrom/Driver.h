#ifndef BOOTROM_DRIVER_H
#define BOOTROM_DRIVER_H

#include <stdint.h>

/**
 * Information structure exported by drivers
 */
struct driver_info_t {
    /// Driver info magic value, must be 'DRVR'
    uint32_t magic;
    /// Name of the driver
    const char *const name;
    /// Driver author
    const char *const author;
    /// Bonus driver description (optional)
    const char *const description;
    /// Version string
    const char *const version;
};

#endif
