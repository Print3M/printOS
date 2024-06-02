/*
	See ACPI Specification:
		For general long description what is going on:
			5. ACPI Software Programming Model
		For structures and essentials start with:
			5.2.5. Root System Description Pointer

	Shortly:
	RSDP.rsdt_addr -> RSDT.sdt_table[] = (example) [APIC, HPET, FACP, MADT]

	Longer:
	Kernel gets the address of ACPI RSDP structure from the bootloader.
	The ACPI RSDP structure has address of RSDT structure.
	RSDT structure ends with table of 32-bit SDT addresses.
	To use some ACPI device/abilities we need to get right SDT structure.
	Every SDT structure starts with standard SDT header but then it has
	custom fields and values for specific ACPI device.
*/
#pragma once
#include <libc/stdint.hpp>

// #define RSDP_SIGNATURE "RSD PTR "
// #define RSDT_SIGNATURE "RSDT"

namespace acpi {

enum Version {
	VER_1	= 0,
	VER_2	= 2,
	UNKNOWN = 10
};
constexpr char const *RSDP_SIG = "RSD PTR ";

// ACP, Table 5.3
struct __attribute__((packed)) RSDP {
	// RSDP for ACPI 1.0
	char signature[8];
	u8 checksum;
	u8 oem_id[6];
	u8 revision;
	u32 rsdt_addr;
};

// ACP, 5.2.3.2. Generic Address Structure
struct __attribute__((packed)) GAS {
	u8 address_space_id;
	u8 reg_bit_width;
	u8 reg_bit_offset;
	u8 access_size;
	u64 address;
};

Version get_version();

bool verify_rsdp();

} // namespace acpi

namespace acpi::sdt {

// All available signatures: ACP, Table 5.5 and 5.6
constexpr char const *HPET_SIG = "HPET";
constexpr char const *MADT_SIG = "APIC";
constexpr char const *RSDT_SIG = "RSDT";

// ACP, Table 5.4
struct __attribute__((packed)) Header {
	char signature[4];
	u32 length;
	u8 revision;
	u8 checksum;
	char oem_id[6];
	char oem_table_id[8];
	u32 oem_revision;
	u32 creator_id;
	u32 creator_revision;
};

// ACP, Table 5.7
struct __attribute__((packed)) RSDT {
	Header header;
	u32 sdt_table[]; // Table of 32-bit addresses of SDT headers
};

bool verify_rsdt();
Header *get_header(const char *signature);

}; // namespace acpi::sdt