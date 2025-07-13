#include <stdint.h>
#include <stddef.h>

void *memset(void *s, int c, size_t n) {
    uint8_t *ptr = (uint8_t *)s;
    while (n--)
        *ptr++ = (uint8_t)c;

    return s;
}