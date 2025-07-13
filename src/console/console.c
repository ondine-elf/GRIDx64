#include "vga.h"
#include "rgb.h"
#include "framebuffer.h"
#include "console.h"

static struct console_driver console_drv;

void console_init(int use_text_mode) {
    if (use_text_mode) {
        console_drv.type = CONSOLE_TYPE_TEXT;
        console_drv.clear = vga_clear;
        console_drv.scroll = vga_scroll;
        console_drv.putc = vga_putc;
    } else {
        console_drv.type = CONSOLE_TYPE_RGB;
        console_drv.clear = rgb_clear;
        console_drv.scroll = rgb_scroll;
        console_drv.putc = rgb_putc;
    }
}

void console_clear(void) {
    console_drv.clear();
}

void console_scroll(void) {
    console_drv.scroll();
}

void console_putc(char c) {
    console_drv.putc(c);
}

void console_puts(const char* str) {
    while (*str) console_putc(*str++);
}


