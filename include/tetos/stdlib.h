#ifndef TETOS_STDLIB_H
#define TETOS_STDLIB_H

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
char *itoa(int value, char *str, int base);
size_t strlen(const char* str);

#endif