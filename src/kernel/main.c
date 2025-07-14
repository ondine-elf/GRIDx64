#include <cpuid.h>
#include <stdint.h>
#include <tetos/io.h>
#include <tetos/stdlib.h>
#include <tetos/multiboot.h>
#include <drivers/console/console.h>
#include <drivers/video/framebuffer.h>
#include <drivers/i8259/i8259.h>
#include <interrupts/idt.h>

void decode_gpf_error(uint32_t code) {
    uint16_t index = (code >> 3) & 0x1FFF;
    uint8_t ti = (code >> 0) & 1;
    uint8_t idt = (code >> 1) & 1;
    uint8_t ext = (code >> 2) & 1;

    console_printf("GPF Error Code:\n");
    console_printf("  Segment Index: %d\n", index);
    console_printf("  Table: %s\n", ti ? "LDT" : "GDT");
    console_printf("  Refers to: %s\n", idt ? "IDT" : "GDT/LDT");
    console_printf("  Source: %s\n", ext ? "External (e.g., interrupt)" : "Software");
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

    console_init();
    pic_remap(0x20, 0x28);
    pic_set_mask(0);
    idt_install();
    
    while (1);
}