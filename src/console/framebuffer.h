#ifndef CONSOLE_FRAMEBUFFER_H
#define CONSOLE_FRAMEBUFFER_H

#include <stdint.h>

struct framebuffer {
    uintptr_t addr;
    uint32_t  pitch;
    uint32_t  width;
    uint32_t  height;
    uint8_t   bpp;
};

extern struct framebuffer fb;

#endif