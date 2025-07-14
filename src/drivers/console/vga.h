#ifndef DRIVERS_CONSOLE_VGA_H
#define DRIVERS_CONSOLE_VGA_H

#include <stdint.h>

#define VGA_MEMORY_ADDR  0xB8000
#define VGA_DEFAULT_ATTR 0x07
#define VGA_MAX_COLS     80
#define VGA_MAX_ROWS     25

enum vga_color {
    VGA_COLOR_BLACK        = 0,
    VGA_COLOR_BLUE         = 1,
    VGA_COLOR_GREEN        = 2,
    VGA_COLOR_CYAN         = 3,
    VGA_COLOR_RED          = 4,
    VGA_COLOR_PURPLE       = 5,
    VGA_COLOR_BROWN        = 6,
    VGA_COLOR_GRAY         = 7,
    VGA_COLOR_DARK_GRAY    = 8,
    VGA_COLOR_LIGHT_BLUE   = 9,
    VGA_COLOR_LIGHT_GREEN  = 10,
    VGA_COLOR_LIGHT_CYAN   = 11,
    VGA_COLOR_LIGHT_RED    = 12,
    VGA_COLOR_LIGHT_PURPLE = 13,
    VGA_COLOR_YELLOW       = 14,
    VGA_COLOR_WHITE        = 15,
};

static inline uint8_t vga_color_attr(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}

static inline uint16_t vga_entry(char c, uint8_t attr) {
    return c | (attr << 8);
}

void vga_clear(void);
void vga_scroll(void);
void vga_putc(char c);

#endif