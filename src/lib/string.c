#include <stdint.h>
#include <stddef.h>

char *itoa(int value, char *str, int base) {
    char temp[33];
    int i = 0, j = 0;
    unsigned int uval;

    if (base < 2 || base > 36) {
        str[0] = '\0';
        return str;
    }

    if (base == 10 && value < 0) {
        uval = (unsigned int)(-value);
    } else {
        uval = (unsigned int)value;
    }

    if (uval == 0) temp[i++] = '0';

    while (uval > 0) {
        int digit = uval % base;
        temp[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        uval /= base;
    }

    if (base == 10 && value < 0) str[j++] = '-';

    for (int k = i - 1; k >= 0; k--) str[j++] = temp[k];
    str[j] = '\0';

    return str;
}

size_t strlen(const char* str) {
    const char* s = str;
    while (*s) s++;
    return s - str;
}

// Might not work for a very large negative value?

