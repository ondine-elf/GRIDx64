#ifndef ACPI_ACPI_H
#define ACPI_ACPI_H

#include <stdint.h>

/* Used to express register address within tables defined by ACPI */
struct GenericAddress {
    uint8_t  AddressSpaceID;
    uint8_t  RegisterBitWidth;
    uint8_t  RegisterBitOffset;
    uint8_t  AccessSize;
    uint64_t Address;
} __attribute__((packed));

/*
* On IA-PC systems, the Root System Description Pointer (RSDP)
* is found by searching physical memory ranges on 16-byte boundaries
* for a valid RSDP structure signature and checksum match
* as follows:
*
* - The first 1 KB of the EBDA. For EISA or MCA systems, the EBDA
*   can be found in the two-byte location 40:0E on the BIOS data area.
*
* - The BIOS read-only memory space between 0xE0000 and 0xFFFFF.
*
* In UEFI systems, a pointer to the RSDP exists within the EFI System Table.
*/
struct RSDP {
    char     Signature[8];
    uint8_t  Checksum;
    char     OEMID[6];
    uint8_t  Revision;
    uint32_t RsdtAddress;

    /* ACPI 2.0+ */
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t  ExtendedChecksum;
    uint8_t  Reserved[3];
} __attribute__((packed));


/*
* All system description tables begin with the following header structure.
* For a list of valid description header signatures, see Tables 5.5 and 5.6
* in the ACPI 6.6 specification.
*/
struct SDTHeader {
    char     Signature[4];
    uint32_t Length;
    uint8_t  Revision;
    uint8_t  Checksum;
    char     OEMID[6];
    char     OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed));

/*
* The "Entries" field is an array of 32-bit physical addresses that
* point to other table description headers.
*/
struct RSDT {
    struct SDTHeader Header;
    uint32_t Entries[];
} __attribute__((packed));

/*
* The "Entries" field is an array of 64-bit physical addresses that
* point to other table description headers.
*/
struct XSDT {
    struct SDTHeader Header;
    uint64_t Entries[];
} __attribute__((packed));

/*
 * Fixed ACPI Description Table (FADT / FACP). See the "BootArchitectureFlags"
 * for relevant fields relating to legacy hardware, the RTC, MSI, etc.
 */
struct FADT {
    struct   SDTHeader Header;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;

    // Used in ACPI 1.0; no longer in use
    uint8_t  Reserved;

    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;

    // Reserved in ACPI 1.0
    uint16_t BootArchitectureFlags;

    uint8_t  Reserved2;
    uint32_t Flags;

    // ACPI 2.0+

    struct GenericAddress ResetReg;

    uint8_t  ResetValue;
    uint8_t  Reserved3[3]; // ARM boot flags and FADT minor version.
  
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;

    struct GenericAddress X_PM1aEventBlock;
    struct GenericAddress X_PM1bEventBlock;
    struct GenericAddress X_PM1aControlBlock;
    struct GenericAddress X_PM1bControlBlock;
    struct GenericAddress X_PM2ControlBlock;
    struct GenericAddress X_PMTimerBlock;
    struct GenericAddress X_GPE0Block;
    struct GenericAddress X_GPE1Block;
} __attribute__((packed));

#define FADT_BOOT_LEGACY_DEVICES       (1 << 0) // Legacy devices (8042, COM1)
#define FADT_BOOT_8042_PRESENT         (1 << 1) // 8042 keyboard controller present
#define FADT_BOOT_VGA_NOT_PRESENT      (1 << 2) // VGA is NOT present
#define FADT_BOOT_MSI_SUPPORTED        (1 << 3) // MSI supported
#define FADT_BOOT_PCIE_ASPM_CONTROLS   (1 << 4) // If set, OS must NOT enable PCIe ASPM
#define FADT_BOOT_CMOS_RTC_PRESENT     (1 << 5) // CMOS RTC present

#define FADT_PWR_PROFILE_UNSPECIFIED        0
#define FADT_PWR_PROFILE_DESKTOP            1
#define FADT_PWR_PROFILE_MOBILE             2
#define FADT_PWR_PROFILE_WORKSTATION        3
#define FADT_PWR_PROFILE_ENTERPRISE         4
#define FADT_PWR_PROFILE_SOHOSERVER         5
#define FADT_PWR_PROFILE_APPLIANCE_PC       6
#define FADT_PWR_PROFILE_PERFORMANCE_SERVER 7


struct DSDT {
    struct SDTHeader Header;
    uint8_t DefinitionBlock[];
} __attribute__((packed));

#endif

// MULTIPLE APIC FLAGS TELL YOU IF DUAL PICS ARE THERE!!!