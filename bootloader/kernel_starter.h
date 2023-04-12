#pragma once
#include "font.h"
#include "gop.h"
#include <efi.h>

#define KERNEL_STACK_SIZE 0x1000

typedef struct {
	EFI_MEMORY_DESCRIPTOR *memory_map;
	UINTN map_sz;
	UINTN descriptor_sz;
	UINT64 entries;
} MemoryData;

void start_kernel(EFI_HANDLE image_handle,
				  Framebuffer *framebuffer,
				  Psf1_font *psf_font,
				  void *kernel_addr,
				  void *acpi_rsdp);