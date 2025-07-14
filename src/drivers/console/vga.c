#include <stdint.h>
#include <tetos/io.h>
#include "vga.h"

static volatile uint16_t *vga_memory = (volatile uint16_t *)VGA_MEMORY_ADDR;
static uint8_t color_attr = VGA_DEFAULT_ATTR;
static int cursor_x = 0;
static int cursor_y = 0;

static void update_cursor(void) {
    int cursor_pos = cursor_y * VGA_MAX_COLS + cursor_x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, cursor_pos & 0xFF);

    outb(0x3D4, 0x0E);
    outb(0x3D5, (cursor_pos >> 8) & 0xFF);
}

void vga_clear(void) {
    uint16_t space = vga_entry(' ', color_attr);
    for (int i = 0; i < VGA_MAX_COLS * VGA_MAX_ROWS; i++)
        vga_memory[i] = space;

    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void vga_scroll(void) {
    for (int i = 0; i < VGA_MAX_COLS * (VGA_MAX_ROWS - 1); i++)
        vga_memory[i] = vga_memory[i + VGA_MAX_COLS];

    for (int i = VGA_MAX_COLS * (VGA_MAX_ROWS - 1); i < VGA_MAX_COLS * VGA_MAX_ROWS; i++)
        vga_memory[i] = vga_entry(' ', color_attr);

    cursor_x = 0;
    cursor_y = VGA_MAX_ROWS - 1;
    update_cursor();
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_x = VGA_MAX_COLS - 1;
            cursor_y--;
        } else return;
        vga_memory[cursor_y * VGA_MAX_COLS + cursor_x] = vga_entry(' ', color_attr);
        return;
    } else if (c >= 32 && c <= 126) {
        vga_memory[cursor_y * VGA_MAX_COLS + cursor_x] = vga_entry(c, color_attr);
        cursor_x++;
        if (cursor_x >= VGA_MAX_COLS) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= VGA_MAX_ROWS) {
        vga_scroll();
        return;
    }

    update_cursor();
}       



