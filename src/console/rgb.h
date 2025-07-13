#ifndef CONSOLE_RGB_H
#define CONSOLE_RGB_H

#include <stdint.h>

#define RGB_CHAR_HEIGHT   16
#define RGB_CHAR_WIDTH    8
#define RGB_DEFAULT_COLOR 0xFF00FF00

struct framebuffer;

void rgb_clear();
void rgb_scroll();
void rgb_putc(char c);

#endif