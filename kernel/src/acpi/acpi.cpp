#include <acpi/acpi.hpp>
#include <kernel.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>
#include <memory/paging.hpp>

namespace acpi {

Version get_version() {
	auto rsdp = kernel.acpi_rsdp;

	if (rsdp->revision == (u8) Version::VER_1) {
		return Version::VER_1;
	}
	if (rsdp->revision == (u8) Version::VER_2) {
		return Version::VER_2;
	}
	return Version::UNKNOWN;
}

bool verify_rsdp() {
	// Check RSDP signature
	auto rsdp = kernel.acpi_rsdp;

	if (strncmp(rsdp->signature, RSDP_SIG, 8) == 0) {
		return true;
	}
	return false;
}

} // namespace acpi

namespace acpi::sdt {

bool verify_rsdt() {
	// Check RSDT signature
	auto rsdp = kernel.acpi_rsdp;
	auto rsdt = (RSDT *) (u64) rsdp->rsdt_addr;

	if (strncmp(rsdt->header.signature, RSDT_SIG, 4) == 0) {
		return true;
	}
	return false;
}

Header *get_header(const char *signature) {
	/*
		Return pointer to the ACPI Standard Description Table Header
		identified by signature. If not found, return null.

		It iterates over sdt_table in RSDT and matches signatures one by one.
		See all possible signatures: ACPI Spec., Table 5.5, Table 5.6
	*/

	auto rsdp	= kernel.acpi_rsdp;
	auto rsdt	= (RSDT *) rsdp->rsdt_addr;
	Header *sdt = nullptr;

	// Calculate size of sdt_table (total amount of 32-bit addresses)
	size entries = (rsdt->header.length - sizeof(RSDT)) / 4;

	for (size i = 0; i < entries; i++) {
		sdt = (Header *) rsdt->sdt_table[i];

		// Check signature
		if (strncmp((const char *) sdt->signature, signature, 4) == 0) {
			return sdt;
		}
	}

	return nullptr;
}

} // namespace acpi::sdt