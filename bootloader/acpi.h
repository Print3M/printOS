/*
	See ACPI Specification, chapter 5.2.5. Root System Description Pointer

	RSDP is pointer to the RSDP structure.
	RSDP structure has field xsdt_addr which is pointer to the table of
	many different System Description Tables (SDT).
	XSDT starts with header STD_HEADER.

*/

#pragma once
#include <efi.h>
#include <efilib.h>

#define EFI_ACPI_TABLE_GUID                                                                \
	{                                                                                  \
		0xeb9d2d30, 0x2d88, 0x11d3, { 0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } \
	}

void *get_rsdp(void);