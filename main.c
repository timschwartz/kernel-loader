#include <stdint.h>
#include <efi.h>
#include <efilib.h>

const CHAR16 *memory_types[] = 
{
    L"EfiReservedMemoryType",
    L"EfiLoaderCode",
    L"EfiLoaderData",
    L"EfiBootServicesCode",
    L"EfiBootServicesData",
    L"EfiRuntimeServicesCode",
    L"EfiRuntimeServicesData",
    L"EfiConventionalMemory",
    L"EfiUnusableMemory",
    L"EfiACPIReclaimMemory",
    L"EfiACPIMemoryNVS",
    L"EfiMemoryMappedIO",
    L"EfiMemoryMappedIOPortSpace",
    L"EfiPalCode",
};

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    UINTN mapSize = 0, mapKey, descriptorSize;
    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
    UINT32 descriptorVersion = 1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    Print(L"Disabling watchdog timer.\n\n");
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

  
    while(EFI_SUCCESS != (result = uefi_call_wrapper((void *)SystemTable->BootServices->GetMemoryMap, 5, &mapSize,
                                                   memoryMap, &mapKey, &descriptorSize, &descriptorVersion)))
    {
        if(result == EFI_BUFFER_TOO_SMALL)
        {
            Print(L"Setting up memory map buffer.\n");
            mapSize += 2 * descriptorSize;
            uefi_call_wrapper((void *)SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, mapSize, (void **)&memoryMap);
        } 
        else Print(L"Error getting memory map: %d.\n", result);
    }

    Print(L"Memory map size: %d.\n", mapSize);
    Print(L"Memory descriptor size: %d.\n\n", descriptorSize);

    uint8_t *startOfMemoryMap = (uint8_t *)memoryMap;
    uint8_t *endOfMemoryMap = startOfMemoryMap + mapSize;

    uint8_t *offset = startOfMemoryMap;

    uint32_t counter = 0; 
    uint64_t totalPages = 0;

    EFI_MEMORY_DESCRIPTOR *desc = NULL;

    while(offset < endOfMemoryMap)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        Print(L"Map %d:\n", counter);
        Print(L"  Type: %X, %s\n", desc->Type, memory_types[desc->Type]); 
        Print(L"  PhysicalStart: %X\n", desc->PhysicalStart);
        Print(L"  VirtualStart: %X\n", desc->VirtualStart);
        Print(L"  NumberOfPages: %X   (4k)\n", desc->NumberOfPages);
        Print(L"  Attribute: %X\n", desc->Attribute);

        totalPages += desc->NumberOfPages;

        offset += descriptorSize;
        counter++;
    }

    uint64_t memorySize = totalPages * 4096;
    Print(L"Memory detected: %d MB\n", memorySize / 1024 / 1024);

    for(;;);

//    Print(L"Exiting boot services.\n");
//    result = uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mapKey);

    return EFI_SUCCESS;
}
