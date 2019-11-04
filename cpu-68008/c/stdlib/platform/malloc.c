/**
 * Provides wrappers around tinyalloc for the malloc/free functions.
 */
#include <stdlib.h>
#include <stddef.h>

#include <tinyalloc.h>

/**
 * Allocates a memory block and returns its address, or NULL if failed.
 */
void *malloc(size_t size) {
  return ta_alloc(size);
}

/**
 * Allocates multiple elements of the same size
 */
void *calloc(size_t count, size_t size) {
  return ta_calloc(count, size);
}

/**
 * Frees a memory block. In case this fails, we sit here forever.
 */
void free(void *block) {
  bool success = ta_free(block);

  if(!success) {
    // TODO: log a message?
    while(1) {}
  }
}
