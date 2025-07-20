#ifndef ACPI_ACPI_H
#define ACPI_ACPI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <efi/efi.h>

struct GenericAddress {
    uint8_t  AddressSpaceID;
    uint8_t  RegisterBitWidth;
    uint8_t  RegisterBitOffset;
    uint8_t  AccessSize;
    uint64_t Address;
} __attribute__((packed));

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

struct RSDT {
    struct SDTHeader Header;
    uint32_t Entries[];
} __attribute__((packed));

struct XSDT {
    struct SDTHeader Header;
    uint64_t Entries[];
} __attribute__((packed));

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

struct MADT {
    struct SDTHeader Header;
    uint32_t LAPICAddress;
    uint32_t Flags;
    uint8_t  Entries[];
} __attribute__((packed));

// Indicates dual 8259 support.
#define MADT_PCAT_COMPAT (1 << 0)

struct madt_entry_header {
    uint8_t type;
    uint8_t length;
} __attribute__((packed));

struct madt_lapic {
    struct madt_entry_header header;
    uint8_t  processor_id;
    uint8_t  apic_id;
    uint32_t flags;
} __attribute__((packed));

#define MADT_LAPIC_ENABLED        (1 << 0)
#define MADT_LAPIC_ONLINE_ENABLED (1 << 1)

struct madt_ioapic {
    struct madt_entry_header header;
    uint8_t  ioapic_id;
    uint8_t  reserved;
    uint32_t ioapic_address;
    uint32_t global_irq_base;
} __attribute__((packed));

bool checksum_valid(const void *addr, size_t len);
struct RSDP *find_rsdp(EFI_SYSTEM_TABLE *system_table);
void *find_acpi_table(const struct RSDP *rsdp, const char *signature);
void acpi_init(EFI_SYSTEM_TABLE *system_table);

struct ACPIInfo {
	struct RSDP *rsdp;
	struct RSDT *rsdt;
	struct XSDT *xsdt;
	struct FADT *fadt;
	struct MADT *madt;

	bool has_apic;
	bool apic_enabled;

	bool has_vga;
	bool has_8259;
	bool has_i8042;

	int numCores;
	int numEntries;
};

extern struct ACPIInfo acpi_info;

#endif

/*
[Page 3465]
The IOAPIC has a "virtual wire mode" that allows it to communicate with a standard 8259A-style
external interrupt controller. Note that the local APIC can be disabled, allowing an associated
processor core to receive interrupts directly from an 8259A interrupt controller.

[Page 3473]
The local APIC state after power-up or reset:
    - IRR, ISR, TMR, ICR, LDR, TPR = 0
    - Timer initial count and timer current count = 0
    - Divide configuration = 0

    - DRF = all 1s
    - LVT = 0s except for mask bits, which are 1
    - SVR = 0x000000FF

[Page 3473]
The local APIC state after it has been software-disabled:
    - It will respond normally to INIT, NMI, SMI, and SIPI message.
    - Pending interrupts in the IRR and ISR are held and require masking
      or handling by the CPU.

    - It can still issue IPIs, although it is the software's responsibility
      to avoid issuing IPIs through the IPI mechanism and the ICR register
      if sending interrupts through this mechanism is not desired.

    - The reception of any interrupt or transmission of any IPIs that are
      in progress when the local APIC is disabled are completed before
      the local APIC enters the software-disabled state.

    - The mask bits for all the LVT entries are set. Attempts to reset these
      bits will be ignored.

[Page 3471]
Enabling and disabling the local APIC:
    - The local APIC can be disabled by setting IA32_APIC_BASE[11] = 0. Upon
      this, the processor is functionally equivalent to an IA-32 processor
      without an on-chip APIC, and the CPUID feature flag for the APIC is set
      to 0. APICs based on the 3-wire APIC bus cannot be re-enabled until a
      system hardware reset, and prior initialization to the APIC may be lost
      and the APIC may return to the state described in Section 12.4.7.1,
      "Local APIC state after power-up or reset."

    - The local APIC can be software-disabled by clearing the APIC software
      enable/disable flag in the spurious-interrupt vector register, upon
      which the APIC's state is described in section 12.4.7.2, "Local APIC
      state after it has been software-disabled." The APIC can be re-enabled
      at any time by setting the APIC software enable/disable flag to 1.

[Page 3471]
IA32_APIC_BASE MSR structure:
    - BSP flag, bit 8: Indicates if the processor is the bootstrap processor
    - APIC Global Enable flag, bit 11: Enables or disables the local APIC. This
      flag is available in the Pentium 4, Intel Xeon, and P6 family processors. It
      is not guaranteed to be available or available at the same location in future
      Intel 64 or IA-32 processors.
    
    - APIC Base field, bits 12 through 35: Specifies the base address of the APIC
      registers. This 24-bit value is extended by 12 bits at the low end to form
      the base address. This automatically aligns the address on a 4-KByte boundary.
      Following a power-up or reset, the field is set to 0xFEE00000

[Page 3466]
PIC 8259 compatibility:
    - The IOAPIC has a "virtual wire mode" that allows it to communicate
      with a standard 8259A-style external interrupt controller.
    - The local APIC can be disabled, allowing an associated processor core
      to receive interrupts directly from an 8259A controller.
    - When the local APIC is global/hardware disabled, the LINT[1:0] pins are configured
      as INTR and NMI pins, respectively. The processor reads from the system
      bus the interrupt vector number provided by an external interrupt controller,
      such as an 8259A. Asserting the NMI pin signals a non-maskable interrupt, which
      is assigned to interrupt vector 2.

*/
