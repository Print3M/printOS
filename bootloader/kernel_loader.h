#pragma once
#include <efi.h>

#define PAGE_SZ 0x1000

void *load_kernel_into_memory(EFI_FILE *kernel_file);
