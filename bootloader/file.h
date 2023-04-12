#pragma once
#include <efi.h>
#include <efilib.h>
#include <elf.h>

EFI_STATUS get_file_info(EFI_FILE *file, EFI_FILE_INFO **file_info);

EFI_FILE *load_file(EFI_FILE *directory, CHAR16 *file_name, EFI_HANDLE image_handle);
