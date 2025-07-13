#include <stdint.h>

void kernel_main() {
    const char* str = "Hello, world!";
    int i = 0;
    while (str[i]) {
        ((short*)0xb8000)[i] = str[i] | 0x0700;
        i++;
    }
}