/**
 * Routines for using the ROM's ability to unpack data
 */
#include "unpack.h"

#include <stdint.h>

/**
 * Unpacks the LZ4 data to the specified output buffer. That buffer must be big
 * enough to fit the entire data.
 */
int unpack_lz4(void *packed, void *output) {
  int ret;

  // trap $f, input in a1, output in a2
  __asm__ __volatile__(
    "\tmoveq #0xf, %%d0\n"
    "\tmove.l %1, %%a1\n"
    "\tmove.l %2, %%a2\n"
    "\ttrap #1\n"
    "\tmove.l %%d0, %0\n"
    : "=m"(ret) /* return code */
    : "g"(packed), "g"(output) /* packed input and output buffer addresses */
    : "d0", "a1", "a2" /* clobber d0, a1-a2 */
  );

  return ret;
}
