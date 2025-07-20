#include <tetos/stdlib.h>
#include <drivers/video/framebuffer.h>
#include "font8x16.h"
#include "rgb.h"

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

static void rgb_putpixel16(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb.width || y >= fb.height) return;

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    uint16_t rgb =
    ((r >> 3) << 11) |
    ((g >> 2) << 5)  |
    (b >> 3);

    uint16_t *pixel = (uint16_t *)(fb.addr + y * fb.pitch + x * 2);
    *pixel = rgb;
}

static void rgb_putpixel24(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb.width || y >= fb.height) return;

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    uint8_t *pixel = (uint8_t *)(fb.addr + y * fb.pitch + x * 3);
    pixel[0] = b;
    pixel[1] = g;
    pixel[2] = r;
}

static void rgb_putpixel32(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb.width || y >= fb.height) return;

    uint32_t *pixel = (uint32_t *)(fb.addr + y * fb.pitch + x * 4);
    *pixel = 0xFF000000 | color;
}

void rgb_clear16() {
    uint16_t *pixel = (uint16_t *)fb.addr;
    uint32_t pixels = (fb.pitch / 2) * fb.height;
    for (uint32_t i = 0; i < pixels; i++)
        pixel[i] = 0;
}

void rgb_clear24() {
    for (uint32_t y = 0; y < fb.height; y++) {
        uint8_t *row = (uint8_t *)(fb.addr + y * fb.pitch);
        memset(row, 0, fb.pitch);
    }
}

void rgb_clear32() {
    uint32_t *pixel = (uint32_t *)fb.addr;
    uint32_t pixels = (fb.pitch / 4) * fb.height;
    for (uint32_t i = 0; i < pixels; i++)
        pixel[i] = 0;

    cursor_x = 0;
    cursor_y = 0;
}

void rgb_scroll() {
    uint8_t *fb_start = (uint8_t *)fb.addr;
    uint32_t row_bytes = fb.pitch;
    uint32_t scroll_bytes = RGB_CHAR_HEIGHT * row_bytes;
    uint32_t total_bytes = row_bytes * fb.height;

    memmove(fb_start, fb_start + scroll_bytes, total_bytes - scroll_bytes);
    memset(fb_start + total_bytes - scroll_bytes, 0, scroll_bytes);
}

static void rgb_drawchar16(int x, int y, char c, uint32_t color) {
    for (int row = 0; row < RGB_CHAR_HEIGHT; row++) {
        uint8_t bits = font8x16[(uint8_t)c][row];
        for (int col = 0; col < RGB_CHAR_WIDTH; col++) {
            if (bits & (1 << (7 - col))) {
                rgb_putpixel16(x + col, y + row, color);
            }
        }
    }
}

static void rgb_drawchar24(int x, int y, char c, uint32_t color) {
    for (int row = 0; row < RGB_CHAR_HEIGHT; row++) {
        uint8_t bits = font8x16[(uint8_t)c][row];
        for (int col = 0; col < RGB_CHAR_WIDTH; col++) {
            if (bits & (1 << (7 - col))) {
                rgb_putpixel24(x + col, y + row, color);
            }
        }
    }
}

static void rgb_drawchar32(int x, int y, char c, uint32_t color) {
    for (int row = 0; row < RGB_CHAR_HEIGHT; row++) {
        uint8_t bits = font8x16[(uint8_t)c][row];
        for (int col = 0; col < RGB_CHAR_WIDTH; col++) {
            if (bits & (1 << (7 - col))) {
                rgb_putpixel32(x + col, y + row, color);
            }
        }
    }
}

void rgb_putc16(char c) {
    uint32_t max_cols = fb.width / RGB_CHAR_WIDTH;
    uint32_t max_rows = fb.height / RGB_CHAR_HEIGHT;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_x = max_cols - 1;
            cursor_y--;
        } else return;
        for (uint32_t row = 0; row < RGB_CHAR_HEIGHT; row++) {
            for (uint32_t col = 0; col < RGB_CHAR_WIDTH; col++) {
                rgb_putpixel16(cursor_x * RGB_CHAR_WIDTH + col, cursor_y * RGB_CHAR_HEIGHT + row, 0);
            }
        }
        return;
    } else if (c >= 32 && c <= 126) {
        rgb_drawchar16(cursor_x * RGB_CHAR_WIDTH, cursor_y * RGB_CHAR_HEIGHT, c, RGB_DEFAULT_COLOR);
        cursor_x++;
        if (cursor_x >= max_cols) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= max_rows)
        rgb_scroll();
}

void rgb_putc24(char c) {
    uint32_t max_cols = fb.width / RGB_CHAR_WIDTH;
    uint32_t max_rows = fb.height / RGB_CHAR_HEIGHT;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_x = max_cols - 1;
            cursor_y--;
        } else return;
        for (uint32_t row = 0; row < RGB_CHAR_HEIGHT; row++) {
            for (uint32_t col = 0; col < RGB_CHAR_WIDTH; col++) {
                rgb_putpixel24(cursor_x * RGB_CHAR_WIDTH + col, cursor_y * RGB_CHAR_HEIGHT + row, 0);
            }
        }
        return;
    } else if (c >= 32 && c <= 126) {
        rgb_drawchar24(cursor_x * RGB_CHAR_WIDTH, cursor_y * RGB_CHAR_HEIGHT, c, RGB_DEFAULT_COLOR);
        cursor_x++;
        if (cursor_x >= max_cols) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= max_rows)
        rgb_scroll();
}

void rgb_putc32(char c) {
    uint32_t max_cols = fb.width / RGB_CHAR_WIDTH;
    uint32_t max_rows = fb.height / RGB_CHAR_HEIGHT;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_x = max_cols - 1;
            cursor_y--;
        } else return;
        for (uint32_t row = 0; row < RGB_CHAR_HEIGHT; row++) {
            for (uint32_t col = 0; col < RGB_CHAR_WIDTH; col++) {
                rgb_putpixel32(cursor_x * RGB_CHAR_WIDTH + col, cursor_y * RGB_CHAR_HEIGHT + row, 0);
            }
        }
        return;
    } else if (c >= 32 && c <= 126) {
        rgb_drawchar32(cursor_x * RGB_CHAR_WIDTH, cursor_y * RGB_CHAR_HEIGHT, c, RGB_DEFAULT_COLOR);
        cursor_x++;
        if (cursor_x >= max_cols) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= max_rows)
        rgb_scroll();
}

// You want to implement clipping