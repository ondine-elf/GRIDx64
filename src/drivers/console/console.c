#include <stdarg.h>
#include <tetos/stdlib.h>
#include <tetos/boot.h>
#include <drivers/video/framebuffer.h>
#include "rgb.h"
#include "console.h"

static void noop_clear(void) {};
static void noop_scroll(void) {};
static void noop_putc(char c) {};

static struct console_driver console_drv = {
    .clear  = noop_clear,
    .scroll = noop_scroll,
    .putc   = noop_putc
};

int console_init(boot_info_t *boot_info) {
    fb.addr   = boot_info->FrameBufferBase;
    fb.pitch  = boot_info->PixelsPerScanLine * 4;
    fb.width  = boot_info->HorizontalResolution;
    fb.height = boot_info->VerticalResolution;
    fb.bpp    = 32;

    if (fb.addr == 0)
        return -1;

    console_drv.clear  = rgb_clear32;
    console_drv.scroll = rgb_scroll;
    console_drv.putc   = rgb_putc32;

    return 0;
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

static void print_number(int value, int base, int width) {
    char buffer[33];
    itoa(value, buffer, base);
    int len = 0;
    const char *num = buffer;

    if (buffer[0] == '-') {
        console_putc('-');
        num = &buffer[1];
        len = strlen(num);
        for (int i = len; i < width; i++) console_putc('0');
        console_puts(num);
    } else {
        len = strlen(num);
        for (int i = len; i < width; i++) console_putc('0');
        console_puts(num);
    }
}

void console_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char ch;
    while ((ch = *fmt++)) {
        if (ch != '%') {
            console_putc(ch);
            continue;
        }

        int width = 0;
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt++ - '0');
        }

        char spec = *fmt++;
        switch (spec) {
            case 'c': {
                char c = (char)va_arg(args, int);
                console_putc(c);
                break;
            }
            case 's': {
                const char *str = va_arg(args, const char*);
                console_puts(str);
                break;
            }
            case 'd': {
                int val = va_arg(args, int);
                print_number(val, 10, width);
                break;
            }
            case 'x': {
                int val = va_arg(args, int);
                print_number(val, 16, width);
                break;
            }
            case '%': {
                console_putc('%');
                break;
            }
            default:
                console_putc('%');
                console_putc(spec);
                break;
        }
    }

    va_end(args);
}

