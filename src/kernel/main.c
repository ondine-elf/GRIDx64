#include <stdint.h>
#include <tetos/multiboot.h>
#include "src/console/console.h"
#include <tetos/stdlib.h>

void print_memory_map(multiboot_info_t* mbi) {
    if (!(mbi->flags & (1 << 6))) {
        console_printf("No memory map provided by bootloader.\n");
        return;
    }

    console_printf("Memory Map:\n");

    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbi->mmap_addr;
    uint32_t mmap_end = mbi->mmap_addr + mbi->mmap_length;

    while ((uint32_t)mmap < mmap_end) {
        const char* type_str;
        switch (mmap->type) {
            case 1: type_str = "Available"; break;
            case 2: type_str = "Reserved"; break;
            case 3: type_str = "ACPI Reclaimable"; break;
            case 4: type_str = "ACPI NVS"; break;
            case 5: type_str = "Bad Memory"; break;
            default: type_str = "Unknown"; break;
        }

        console_printf("Base: 0x%08x%08x | Length: 0x%08x%08x | Type: %s\n",
            (uint32_t)(mmap->addr >> 32), (uint32_t)(mmap->addr),
            (uint32_t)(mmap->len >> 32), (uint32_t)(mmap->len),
            type_str
        );

        mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
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

        if (console_init(&fb) == 0) {
            
            console_printf("Hello, world!\n");
            print_memory_map(mbi);

        }
    }
}