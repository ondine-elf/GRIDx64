#ifndef TETOS_BOOT_H
#define TETOS_BOOT_H

#include <efi/efi.h>

typedef struct {
    UINT64 FrameBufferBase;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    UINT32 PixelsPerScanLine;

    VOID  *SystemTable;

    VOID  *MemoryMap;
    UINTN  MemoryMapSize;
    UINTN  DescriptorSize;
    UINT32 DescriptorVersion;
} boot_info_t;

#endif