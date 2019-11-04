#ifndef PLATFORM_UNPACK_H
#define PLATFORM_UNPACK_H

/**
 * Unpacks the LZ4 data to the specified output buffer. That buffer must be big
 * enough to fit the entire data.
 */
int unpack_lz4(void *packed, void *output);

#endif
