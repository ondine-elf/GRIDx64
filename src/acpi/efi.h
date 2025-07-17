#ifndef ACPI_EFI_H
#define ACPI_EFI_H

#include <stdint.h>
#include <stdbool.h>
#include <tetos/stdlib.h>

// Page 22
typedef uint8_t BOOLEAN;

typedef intptr_t  INTN;
typedef uintptr_t UINTN;

typedef int8_t    INT8;
typedef uint8_t   UINT8;
typedef int16_t   INT16;
typedef uint16_t  UINT16;
typedef int32_t   INT32;
typedef uint32_t  UINT32;
typedef int64_t   INT64;
typedef uint64_t  UINT64;

typedef __int128_t  INT128;
typedef __uint128_t UINT128;

typedef char     CHAR8;
typedef uint16_t CHAR16;

typedef void VOID;

typedef struct {          
    UINT32  Data1;
    UINT16  Data2;
    UINT16  Data3;
    UINT8   Data4[8]; 
} EFI_GUID;

typedef UINTN EFI_STATUS;
typedef VOID *EFI_HANDLE;
typedef VOID *EFI_EVENT;

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

// Page 98
typedef struct {
    EFI_GUID VendorGuid;
    VOID    *VendorTable;
} EFI_CONFIGURATION_TABLE;

// Page 91
typedef struct {
    EFI_TABLE_HEADER Hdr;

    CHAR16                  *FirmwareVendor;
    UINT32                   FirmwareRevision;
    EFI_HANDLE               ConsoleInHandle;
    VOID                    *ConIn;
    EFI_HANDLE               ConsoleOutHandle;
    VOID                    *ConOut;
    EFI_HANDLE               StandardErrorHandle;
    VOID                    *StdErr;
    VOID                    *RuntimeServices;
    VOID                    *BootServices;
    UINTN                    NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE *ConfigurationTable;
} EFI_SYSTEM_TABLE;

#define ACPI_TABLE_GUID \
    {0xeb9d2d30,0x2d88,0x11d3,\
    {0x9a,0x16,0x00,0x90,0x27,0x3f,0xc1,0x4d}}

#define EFI_ACPI_20_TABLE_GUID \
    {0x8868e871,0xe4f1,0x11d3,\
    {0xbc,0x22,0x00,0x80,0xc7,0x3c,0x88,0x81}}

int guid_equal(const EFI_GUID *a, const EFI_GUID *b);
void *efi_find_rsdp(EFI_SYSTEM_TABLE *system_table);

extern const EFI_GUID AcpiTableGuid;
extern const EFI_GUID Acpi20TableGuid;

#endif