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
#include <efi/efi.h>
#include <efi/efilib.h>

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

static inline void serial_print(const char *str) {
    while (*str) outb(0x3F8, *str++);
}

void kernel_main(uint32_t magic, uint32_t addr) {

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) return;

    struct multiboot_tag *tag;
    //uint32_t size;

    // PASSES FRAMEBUFFER AND EFI64!!! Be wary, if you don't request framebuffer it'll try console by fail
    for (tag = (struct multiboot_tag *)((uintptr_t)addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {

            switch (tag->type) {
                case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                    
                    serial_print("Received framebuffer tag\r\n");

                    // {addr = 3221225472, pitch = 4096, width = 1024, height = 768, bpp = 32, type = 8}
                    struct multiboot_tag_framebuffer *mbf = (struct multiboot_tag_framebuffer *)tag;
                    fb.addr = mbf->common.framebuffer_addr;
                    fb.bpp = mbf->common.framebuffer_bpp;
                    fb.pitch = mbf->common.framebuffer_pitch;
                    fb.type = mbf->common.type; 
                    fb.width = mbf->common.framebuffer_width;
                    fb.height = mbf->common.framebuffer_height;

                    console_init();
                    console_printf("Hello, world!\n");
                    
                    break;
                }
                case MULTIBOOT_TAG_TYPE_EFI32: {
                    serial_print("Received EFI32 tag\r\n");
                    break;
                }
                case MULTIBOOT_TAG_TYPE_EFI64: {

                    serial_print("Received EFI64 tag\r\n");

                    struct multiboot_tag_efi64 *efi64 = (struct multiboot_tag_efi64 *)tag;
                    void *efi_system_table = (void *)(uintptr_t)efi64->pointer;
                    EFI_SYSTEM_TABLE *table = (EFI_SYSTEM_TABLE *)efi_system_table;

                    struct RSDP *rsdp = NULL;
                    for (uint64_t i = 0; i < table->NumberOfTableEntries; i++) {
                        EFI_GUID *guid = &table->ConfigurationTable[i].VendorGuid;

                        EFI_GUID acpi20 = ACPI_20_TABLE_GUID;
                        EFI_GUID acpi10 = ACPI_TABLE_GUID;
                        if (memcmp(guid, &acpi20, sizeof(EFI_GUID)) == 0 || memcmp(guid, &acpi10, sizeof(EFI_GUID)) == 0) {
                            rsdp = table->ConfigurationTable[i].VendorTable;
                        }
                    }

                    if (rsdp) {
                        uint64_t rsdp_addr = (uint64_t)(uintptr_t)rsdp;
                        console_printf("ACPI: RSDP 0x%08x%08x\n",
                            (uint32_t)(rsdp_addr >> 32),
                            (uint32_t)(rsdp_addr & 0xFFFFFFFF));

                        struct RSDP *rsdp_ptr = (struct RSDP *)rsdp;
                        int length;

                        if (rsdp_ptr->Revision == 1) {
                            length = 20;
                            console_printf("ACPI: Revision 1.0\n");
                        } else {
                            length = 36;
                            console_printf("ACPI: Revision 2.0+\n");
                        }

                        if (memcmp((char*)rsdp_ptr, "RSD PTR ", 8) != 0) {
                            console_printf("RSDP signature mismatch! Please check your firmware.\n");
                            return;
                        }

                        uint8_t checksum = 0;
                        for (int i = 0; i < length; i++)
                            checksum += ((uint8_t *)rsdp)[i];
                        if (checksum != 0) {
                            console_printf("ACPI: Checksum not valid! Please check your firmware.\n");
                            return;
                        }

                        
                        int numCores = 0;
                        struct XSDT *xsdt = (struct XSDT *)rsdp->XsdtAddress;
                        int numEntries = (xsdt->Header.Length - sizeof(struct SDTHeader)) / 8;
                        uint64_t *entries = xsdt->Entries;
                        for (int i = 0; i < numEntries; i++) {
                            struct SDTHeader *header = (struct SDTHeader *)entries[i];
                            char sig[5];
                            sig[4] = '\0';
                            memmove(sig, header->Signature, 4);
                            console_printf("ACPI: %s\n", sig);
                            
                            if (memcmp(header->Signature, "APIC", 4) == 0) {
                                struct MADT *madt = (struct MADT *)(uintptr_t)entries[i];
                                uint8_t *ptr = (uint8_t*)madt + sizeof(struct MADT);
                                uint8_t *end = (uint8_t*)madt + madt->Header.Length;

                                while (ptr < end) {
                                    struct madt_entry_header *entry = ptr;

                                    if (entry->type == 0) {
                                        struct madt_lapic *lapic = entry;
                                        if (lapic->flags & 1) numCores++;
                                    }

                                    ptr += entry->length;
                                }

                                
                            } 
                        }

                        console_printf("CPU Cores: %d\n", numCores);

                        unsigned int eax, ebx, ecx, edx;
                        if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                            if (edx & (1 << 9)) {
                                console_printf("APIC: Detected\n");
                                if (rdmsr(0x1B) & (1 << 11)) {
                                    console_printf("APIC: Enabled\n");
                                } else {
                                    uint64_t msr = rdmsr(0x1B);
                                    msr |= (1 << 11);
                                    wrmsr(0x1B, msr);
                                    console_printf("APIC: Enabled\n");
                                }
                            }
                        }
                        
                        
                    }
                    
                    break;
                }
                default:
                break;
            }
            
        }
        
        
    #ifdef CONFIG_BIOS

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
    pic_clear_mask(1);
    //idt_install();
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

    if (!rsdp) {
        console_printf("WARN: No RSDP found.\n");
    } else {
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
    }

    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        if (edx & (1 << 9)) {
            console_printf("CPUID: APIC supported detected\n");

            uint64_t msr = rdmsr(0x1B);
            if (msr & (1 << 11)) console_printf("ACPI: APIC enabled\n");

            volatile uint32_t *lapic = (volatile uint32_t *)0xFEE00000;
            uint32_t lint0 = lapic[0x350 / 4];
            uint32_t delivery_mode = (lint0 >> 8) & 0x7;
            if (delivery_mode == 0x7)
                console_printf("LAPIC LINT0 is configured as ExtINT\n");
            else
                console_printf("LAPIC LINT0 delivery mode: %d\n", delivery_mode);
        }
    }

    #endif
}

// APIC enabled & LINT0 ExtINT: Keyboard works as long as IRQ 1 is unmasked via 8259
// APIC enabled & LINT0 masked: Keyboard will NOT work
// APIC disabled & LINT0 ExtINT: Keyboard works as long as IRQ 1 is unmasked via 8259
// APIC disabled & LINT0 masked: Keyboard will NOT work
// APIC enabled & changed delivery mode for LINT0: Keyboard will NOT work
// APIC disabled & changed delivery mode for LINT0: Keyboard will NOT work. Why??????

/*
* - Trying to use IRQs in BIOS without remapping the PIC will cause a fault
* - 
*/

// WHY ARE THERE MULTIPLE RSD PTR IN MEMORY?