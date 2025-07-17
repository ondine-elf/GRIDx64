#include "efi.h"

const EFI_GUID AcpiTableGuid = ACPI_TABLE_GUID;
const EFI_GUID Acpi20TableGuid = EFI_ACPI_20_TABLE_GUID;

int guid_equal(const EFI_GUID *a, const EFI_GUID *b) {
    return memcmp(a, b, sizeof(EFI_GUID)) == 0;
}

void *efi_find_rsdp(EFI_SYSTEM_TABLE *system_table) {
    for (int i = 0; i < system_table->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *config = &system_table->ConfigurationTable[i];

       if (guid_equal(&config->VendorGuid, &AcpiTableGuid) || guid_equal(&config->VendorGuid, &Acpi20TableGuid)) {
           return (void *)config->VendorTable;
       }
    }
    return NULL;
}