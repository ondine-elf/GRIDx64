#include <cpuid.h>
#include <stdint.h>
#include <tetos/io.h>
#include <tetos/stdlib.h>
#include <tetos/multiboot.h>
#include <drivers/console/console.h>
#include <drivers/video/framebuffer.h>

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

    console_init();
    console_puts("Hello, world!\n");
}