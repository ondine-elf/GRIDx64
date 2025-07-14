#ifndef CONSOLE_RGB_H
#define CONSOLE_RGB_H

#include <stdint.h>

#define RGB_CHAR_HEIGHT   16
#define RGB_CHAR_WIDTH    8
#define RGB_DEFAULT_COLOR 0xFFFFFFFF

void rgb_scroll(void);

void rgb_clear16(void);
void rgb_putc16(char c);

void rgb_clear24(void);
void rgb_putc24(char c);

void rgb_clear32(void);
void rgb_putc32(char c);

#endif