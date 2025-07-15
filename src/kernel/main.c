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

#include <acpi/acpi.h>

struct ACPI {
    struct RSDP      *rsdp;
    struct SDTHeader *rsdt;
    struct SDTHeader *facp;
    struct SDTHeader *apic;
    struct SDTHeader *hpet;
} __attribute__((packed));

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
    asm volatile ("sti");

    struct RSDP *rsdp = NULL;

    uint16_t ebda_segment = *((uint16_t *)0x40E);
    uint32_t ebda_addr = (uint32_t)ebda_segment << 4;
    for (uint32_t addr = ebda_addr; addr < ebda_addr + 1024; addr += 16) {
        if (memcmp((char*)addr, "RSD PTR ", 8) == 0) {
            console_printf("ACPI: RSDP 0x%08x\n", (uint32_t)addr);
            rsdp = (struct RSDP *)addr;
        }
    }

    for (uint32_t addr = 0xE0000; addr < 0xFFFFF; addr += 16) {
        if (memcmp((char*)addr, "RSD PTR ", 8) == 0) {
            console_printf("ACPI: RSDP 0x%08x\n", (uint32_t)addr);
            rsdp = (struct RSDP *)addr;
        }
    }

    uint8_t checksum = 0;
    for (size_t i = 0; i < 20; i++)
        checksum += ((uint8_t *)rsdp)[i];

    if (checksum) console_printf("ERROR! Checksum not valid.\n");

    struct RSDT *rsdt = (struct RSDT *)rsdp->RsdtAddress;
    int numEntries = (rsdt->Header.Length - 36) / 4;
    for (int i = 0; i < numEntries; i++) {
        struct SDTHeader *header = (struct SDTHeader *)rsdt->Entries[i];
        char sig[5] = {0};
        memmove(sig, header->Signature, 4);
        console_printf("ACPI: %s 0x%08x\n", sig, (uint32_t)header);

        

    }

    while (1);
}