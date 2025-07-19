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

typedef struct {
    uint64_t FrameBufferBase;
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelsPerScanLine;

    void *system_table;
} boot_info_t;

static inline void serial_print(const char *str) {
    while (*str) outb(0x3F8, *str++);
}

void kernel_main(uint32_t magic, uint32_t addr) {

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) return;
    EFI_SYSTEM_TABLE *sys_table; // remove ?

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
                    sys_table = efi_system_table; // remove ?

                    acpi_init(efi_system_table);

                    break;
                }
                case MULTIBOOT_TAG_TYPE_MMAP: {
                    multiboot_memory_map_t *mmap;
                    for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
                        (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                         mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size)) {
                            console_printf("Base: 0x%08x%08x, Length: 0x%08x%08x, Type=0x%08x\n",
                                (unsigned)(mmap->addr >> 32),
                                (unsigned)(mmap->addr & 0xFFFFFFFF),
                                (unsigned)(mmap->len >> 32),
                                (unsigned)(mmap->len & 0xFFFFFFFF),
                                (unsigned)mmap->type);
                         }
                    break;
                }
                case MULTIBOOT_TAG_TYPE_EFI_MMAP: {
                    break;
                }
                default: {
                    break;
                }
            }
    }
    
    if (sys_table == NULL)
        return;

    EFI_GUID AcpiTableGuid = ACPI_TABLE_GUID;
    EFI_GUID Acpi20TableGuid = ACPI_20_TABLE_GUID;

    for (UINTN i = 0; i < sys_table->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *config_table = &sys_table->ConfigurationTable[i];
        if (memcmp(&config_table->VendorGuid, &AcpiTableGuid, sizeof(EFI_GUID)) == 0) {
            console_printf("Found ACPI 1.0 at 0x%08x\n", (uintptr_t)(void *)config_table);
            struct RSDP *rsdp = (struct RSDP *)config_table->VendorTable;
            console_printf("ACPI 1.0 Revision: %d\n", rsdp->Revision);
        }
        
        if (memcmp(&config_table->VendorGuid, &Acpi20TableGuid, sizeof(EFI_GUID)) == 0) {
            console_printf("Found ACPI 2.0 at 0x%08x\n", (uintptr_t)(void *)config_table);
            struct RSDP *rsdp = (struct RSDP *)config_table->VendorTable;
            console_printf("ACPI 2.0 Revision: %d\n", rsdp->Revision);
        }
    }

    // ACPI 1.0 @ 0xBF3ECD58
    // ACPI 2.0 @ 0xBF3ECD70

}