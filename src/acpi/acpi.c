#include "acpi.h"
#include <efi/efi.h>
#include <tetos/stdlib.h>
#include <cpuid.h>
#include <tetos/io.h>
#include <drivers/console/console.h>

struct ACPIInfo acpi_info = {0};

bool checksum_valid(const void *addr, size_t len) {
    const uint8_t *bytes = (const uint8_t *)addr;
    uint8_t sum = 0;

    for (size_t i = 0; i < len; i++)
        sum += bytes[i];

    return sum == 0;
}

// This returns the first value by default you fat STUPID IDIOT!!!! YOU JUST RETURNED
// THE FIRST ONE!!!
struct RSDP *find_rsdp(EFI_SYSTEM_TABLE *system_table) {
    if (system_table == NULL)
        return NULL;

    const EFI_GUID AcpiTableGuid = ACPI_TABLE_GUID;
    const EFI_GUID Acpi20TableGuid = ACPI_20_TABLE_GUID;

    for (UINTN i = 0; i < system_table->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *config_table = &system_table->ConfigurationTable[i];

        if (memcmp(&config_table->VendorGuid, &Acpi20TableGuid, sizeof(EFI_GUID)) == 0 ||
            memcmp(&config_table->VendorGuid, &AcpiTableGuid, sizeof(EFI_GUID)) == 0) {
            
            struct RSDP *rsdp = (struct RSDP *)config_table->VendorTable;

            if (rsdp->Revision >= 2) {
                if (checksum_valid(rsdp, rsdp->Length))
                return rsdp;
            } else {
                if (checksum_valid(rsdp, 20))
                    return rsdp;
            }
        }
    }

    return NULL;
}

// CANNOT UWORK FOR RSDT AND XSDT!!!
void *find_acpi_table(const struct RSDP *rsdp, const char *signature) {
    if (rsdp == NULL || signature == NULL)
        return NULL;

    if (rsdp->Revision >= 2 && rsdp->XsdtAddress != 0) {
        struct XSDT *xsdt = (struct XSDT *)(uintptr_t)rsdp->XsdtAddress;
        if (!checksum_valid(xsdt, xsdt->Header.Length))
            return NULL;

        int entries = (xsdt->Header.Length - sizeof(xsdt->Header)) / sizeof(uint64_t);
        for (int i = 0; i < entries; i++) {
            struct SDTHeader *hdr = (struct SDTHeader *)(uintptr_t)xsdt->Entries[i];
            if (memcmp(hdr->Signature, signature, 4) == 0 &&
                checksum_valid(hdr, hdr->Length))
                return (void *)hdr;
        }
    } else if (rsdp->RsdtAddress != 0) {
        struct RSDT *rsdt = (struct RSDT *)(uintptr_t)rsdp->RsdtAddress;
        if (!checksum_valid(rsdt, rsdt->Header.Length))
            return NULL;

        int entries = (rsdt->Header.Length - sizeof(rsdt->Header)) / sizeof(uint32_t);
        for (int i = 0; i < entries; i++) {
            struct SDTHeader *hdr = (struct SDTHeader *)(uintptr_t)rsdt->Entries[i];
            if (memcmp(hdr->Signature, signature, 4) == 0 &&
                checksum_valid(hdr, hdr->Length))
                return (void *)hdr;
        }
    }

    return NULL;
}

void acpi_init(EFI_SYSTEM_TABLE *system_table) {
    acpi_info.rsdp = (struct RSDP *)find_rsdp(system_table);

    acpi_info.rsdt = (struct RSDT *)(uintptr_t)acpi_info.rsdp->RsdtAddress;
    if (acpi_info.rsdp->Revision > 1) {
        acpi_info.xsdt = (struct XSDT *)(uintptr_t)acpi_info.rsdp->XsdtAddress;
    } else acpi_info.xsdt = NULL;

    acpi_info.fadt = (struct FADT *)find_acpi_table(acpi_info.rsdp, "FACP");
    acpi_info.madt = (struct MADT *)find_acpi_table(acpi_info.rsdp, "APIC");

    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        if (edx & (1 << 9)) {
            acpi_info.has_apic = 1;
            uint64_t apic_base_msr = rdmsr(0x1B);
            if (apic_base_msr & (1 << 11))
                acpi_info.apic_enabled = 1;
        }
    }

    if (acpi_info.fadt->BootArchitectureFlags & FADT_BOOT_8042_PRESENT)
        acpi_info.has_i8042 = 1;
    if (!(acpi_info.fadt->BootArchitectureFlags & FADT_BOOT_VGA_NOT_PRESENT))
        acpi_info.has_vga = 1;

    if (acpi_info.madt->Flags & MADT_PCAT_COMPAT)
        acpi_info.has_8259 = 1;

    uint8_t *ptr = (uint8_t *)acpi_info.madt->Entries;
    uint8_t *end = (uint8_t *)((uintptr_t)acpi_info.madt->Entries + acpi_info.madt->Header.Length);

    while (ptr < end) {
        struct madt_entry_header *header = (struct madt_entry_header *)ptr;
        if (header->type == 0) {
            struct madt_lapic *lapic = (struct madt_lapic *)header;
            if (lapic->flags & MADT_LAPIC_ENABLED)
                acpi_info.numCores++;
        }
        ptr += header->length;
    }
    
    console_printf("ACPI: Version %x\n", acpi_info.rsdp->Revision);
    if (acpi_info.rsdp->Revision <= 1) {
        acpi_info.numEntries = (acpi_info.rsdt->Header.Length - sizeof(acpi_info.rsdt->Header)) / sizeof(uint32_t);
        for (int i = 0; i < acpi_info.numEntries; i++) {
            struct SDTHeader *header = (struct SDTHeader *)(uintptr_t)acpi_info.rsdt->Entries[i];
            char signature[5] = {0};
            memmove(signature, header->Signature, 4);
            console_printf("ACPI: %s 0x%08x - 0x%08x\n", signature, (uintptr_t)header, (uintptr_t)header + header->Length);
        }
    } else {
        acpi_info.numEntries = (acpi_info.xsdt->Header.Length - sizeof(acpi_info.xsdt->Header)) / sizeof(uint64_t);
        for (int i = 0; i < acpi_info.numEntries; i++) {
            struct SDTHeader *header = (struct SDTHeader *)acpi_info.xsdt->Entries[i];
            char signature[5] = {0};
            memmove(signature, header->Signature, 4);
            console_printf("ACPI: %s 0x%08x - 0x%08x\n", signature, (uintptr_t)header, (uintptr_t)header + header->Length);
        }
    }
    
    if (acpi_info.has_apic) {
        console_printf("APIC Status: Detected\n");
        if (acpi_info.apic_enabled) console_printf("APIC Status: Enabled\n");
    } else console_printf("APIC Status: Not Detected\n");
    
    if (acpi_info.has_vga) {
        console_printf("VGA Status: Detected\n");
    } else console_printf("VGA Status: Not Detected\n");
    
    if (acpi_info.has_i8042) {
        console_printf("I8042 Status: Detected\n");
    } else console_printf("I8042 Status: Not Detected\n");
    
    if (acpi_info.has_8259) {
        console_printf("Dual 8259 Status: Detected\n");
    } else console_printf("Dual 8259 Status: Not Detected\n");

    console_printf("CPU Cores Detected: %d\n", acpi_info.numCores);
}


