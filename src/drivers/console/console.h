#ifndef DRIVERS_CONSOLE_CONSOLE_H
#define DRIVERS_CONSOLE_CONSOLE_H

#include <stdint.h>

enum console_type {
    CONSOLE_TYPE_SERIAL = 0,
    CONSOLE_TYPE_TEXT   = 1,
    CONSOLE_TYPE_RGB    = 2,
    CONSOLE_TYPE_NONE   = 3,
};

struct console_driver {
    enum console_type type;

    void (*clear)(void);
    void (*scroll)(void);
    void (*putc)(char c);
};

int console_init();
void console_clear(void);
void console_scroll(void);
void console_putc(char c);
void console_puts(const char *str);
void console_printf(const char* fmt, ...);

#endif