#include <cpuid.h>
#include <stdint.h>
#include <tetos/io.h>
#include <tetos/stdlib.h>
//#include <tetos/multiboot.h>
#include <drivers/console/console.h>
#include <drivers/video/framebuffer.h>
#include <drivers/i8259/i8259.h>
//#include <interrupts/idt.h>
#include <tetos/multiboot2.h>

#include <acpi/acpi.h>
#include <efi/efi.h>
#include <efi/efilib.h>

static inline void serial_print(const char *str) {
    while (*str) outb(0x3F8, *str++);
}

void kernel_main(uint32_t magic, uint32_t addr) {

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) return;

    struct multiboot_tag *tag;

    // PASSES FRAMEBUFFER AND EFI64!!! Be wary, if you don't request framebuffer it'll try console by fail
    for (tag = (struct multiboot_tag *)((uintptr_t)addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {

            switch (tag->type) {
                case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                    // {addr = 3221225472, pitch = 4096, width = 1024, height = 768, bpp = 32, type = 8}
                    struct multiboot_tag_framebuffer *mbf = (struct multiboot_tag_framebuffer *)tag;
                    fb.addr = mbf->common.framebuffer_addr;
                    fb.bpp = mbf->common.framebuffer_bpp;
                    fb.pitch = mbf->common.framebuffer_pitch;
                    fb.type = mbf->common.type; 
                    fb.width = mbf->common.framebuffer_width;
                    fb.height = mbf->common.framebuffer_height;

                    console_init();                    
                    break;
                }
                case MULTIBOOT_TAG_TYPE_EFI64: {
                    struct multiboot_tag_efi64 *efi64 = (struct multiboot_tag_efi64 *)tag;
                    EFI_SYSTEM_TABLE *efi_system_table = (EFI_SYSTEM_TABLE *)efi64->pointer;

                    acpi_init(efi_system_table);
                }
                default: {
                    break;
                }
            }
    }         
}