#ifndef STRUTIL_H
#define STRUTIL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
uint8_t strcmp(char *s1, char *s2, uint64_t c);
uint64_t strlen(const char *str);
void *memcpy(void *dest, const void *src, size_t n);
#endif