#ifndef DRIVERS_CONSOLE_FRAMEBUFFER_H
#define DRIVERS_CONSOLE_FRAMEBUFFER_H

#include <stdint.h>

enum framebuffer_type {
    FB_TYPE_INDEXED = 0,
    FB_TYPE_RGB     = 1,
    FB_TYPE_TEXT    = 2,
};

struct framebuffer {
    uintptr_t             addr;
    uint32_t              pitch;
    uint32_t              width;
    uint32_t              height;
    uint8_t               bpp;
    enum framebuffer_type type;
};

extern struct framebuffer fb;

#endif