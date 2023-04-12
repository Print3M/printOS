#include "acpi.h"
#include <efi.h>
#include <efilib.h>

void *get_rsdp(void) {
	// RSDP - Root System Description Pointer
	// Read: ACPI Specification, chapter 5.2.5.

	EFI_CONFIGURATION_TABLE *ct_entry = ST->ConfigurationTable;
	EFI_GUID acpi_table_guid = ACPI_TABLE_GUID;

	for (UINT64 i = 0; i < ST->NumberOfTableEntries; i++) {
		if (CompareGuid(&ct_entry->VendorGuid, &acpi_table_guid) == 0) {
			return ct_entry->VendorTable;
		}

		ct_entry += 1;
	}

	return NULL;
}