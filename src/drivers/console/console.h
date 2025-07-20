#ifndef DRIVERS_CONSOLE_CONSOLE_H
#define DRIVERS_CONSOLE_CONSOLE_H

struct console_driver {
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