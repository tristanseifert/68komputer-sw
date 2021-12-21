#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memchr(const void* src, int c, size_t n);
int memcmp(const void *p1, const void *p2, size_t n);
void *memcpy(void *dst, const void *src, size_t length);
void *memmove(void *dst, const void *src, size_t length);
void *memmem(const void *l, size_t l_len, const void *s, size_t s_len);
void *memset(void *dst, int c, size_t length);

char *strchr(const char *p, int ch);

char *strncat(char *dst, const char *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dst, const char *src, size_t n);
size_t strnlen(const char *str, size_t maxlen);
char* strtok(char *s, const char *delim);

#ifdef __cplusplus
}
#endif

