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
    UINT64 FrameBufferBase;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    UINT32 PixelsPerScanLine;

    VOID  *SystemTable;

    VOID  *MemoryMap;
    UINTN MemoryMapSize;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
} boot_info_t;

static inline void serial_print(const char *str) {
    while (*str) outb(0x3F8, *str++);
}
// YOU CANT FUCK UP THE SIGNATURE!! ITS A POINTER NOT A REFERENCE YOU FAT UGLY LOSER!!!!
void kernel_main(boot_info_t boot_info) {

    // {addr = 3221225472, pitch = 4096, width = 1024, height = 768, bpp = 32, type = 8}
    fb.addr = boot_info.FrameBufferBase;
    fb.bpp = 32;
    fb.pitch = boot_info.PixelsPerScanLine * 4;
    fb.width = boot_info.HorizontalResolution;
    fb.height = boot_info.VerticalResolution;
    fb.type = 8;

    console_init();

    console_printf("Hello, world\n");
    console_clear();
                        
    
    EFI_SYSTEM_TABLE *efi_system_table = (EFI_SYSTEM_TABLE *)boot_info.SystemTable;
    acpi_init(efi_system_table);
    

    EFI_GUID AcpiTableGuid = ACPI_TABLE_GUID;
    EFI_GUID Acpi20TableGuid = ACPI_20_TABLE_GUID;

    for (UINTN i = 0; i < efi_system_table->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *config_table = &efi_system_table->ConfigurationTable[i];
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

    EFI_MEMORY_DESCRIPTOR *mmap = (EFI_MEMORY_DESCRIPTOR *)boot_info.MemoryMap;
    UINTN mmap_entries = boot_info.MemoryMapSize / boot_info.DescriptorSize;
    UINT8 *ptr = (UINT8 *)mmap;

    for (UINTN i = 0; i < mmap_entries; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)(ptr + i * boot_info.DescriptorSize);

        if (desc->Type == EfiConventionalMemory ||
            desc->Type == EfiLoaderCode ||
            desc->Type == EfiLoaderData) {

            UINT64 phys_start = desc->PhysicalStart;
            UINT64 size_bytes = desc->NumberOfPages * 4096;

            console_printf("Usable region: Type=%d Start=0x%08x Size=%08x KB\n",
                        desc->Type,
                        phys_start,
                        size_bytes / 1024);
        }
    }

    while (1);


    // ACPI 1.0 @ 0xBF3ECD58
    // ACPI 2.0 @ 0xBF3ECD70
    // PASSES FRAMEBUFFER AND EFI64!!! Be wary, if you don't request framebuffer it'll try console by fail, for multiboot 2


}