#include <stdint.h>
#include <stddef.h>

void *memset(void *s, int c, size_t n) {
    uint8_t *ptr = (uint8_t *)s;
    while (n--)
        *ptr++ = (uint8_t)c;

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (d == s || n == 0) return dest;

    if (d < s) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else {
        for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
    }

    return dest;
}