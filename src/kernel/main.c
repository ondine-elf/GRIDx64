#include <cpuid.h>
#include <stdint.h>
#include <tetos/io.h>
#include <tetos/stdlib.h>
#include <tetos/multiboot.h>
#include "src/console/console.h"

#define MASTER_PIC_COMMAND 0x20
#define MASTER_PIC_DATA    0x21
#define SLAVE_PIC_COMMAND  0xA0
#define SLAVE_PIC_DATA     0xA1

void pic_remap(int offset1, int offset2) {
    outb(0x20, 0x11);
    io_wait();
    outb(0xA0, 0x11);
    io_wait();

    outb(0x21, offset1);
    io_wait();
    outb(0xA1, offset2);
    io_wait();

    outb(0x21, 1 << 2);
    io_wait();
    outb(0xA1, 2);
    io_wait();

    outb(0x21, 1);
    io_wait();
    outb(0xA1, 1);
    io_wait();

    outb(0x21, 0);
    outb(0xA1, 0);
}

void pci_disable() {
    outb(0x21, 0xff);
    outb(0xA2, 0xff);
}



void kernel_main(uint32_t magic, uint32_t mbi_addr) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) return;
    multiboot_info_t* mbi = (multiboot_info_t *)mbi_addr;

    if (mbi->flags & (1 << 12)) {
        fb.addr = mbi->framebuffer_addr;
        fb.pitch = mbi->framebuffer_pitch;
        fb.width = mbi->framebuffer_width;
        fb.height = mbi->framebuffer_height;
        fb.bpp = mbi->framebuffer_bpp;
        fb.type = mbi->framebuffer_type;
    }

    console_init(&fb);
    console_puts("Hello, world!\n");

}