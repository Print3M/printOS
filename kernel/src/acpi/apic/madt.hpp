/*
	Multiple APIC Description Table (MADT).
	Read more in ACP, chapter 5.2.12.
*/
#pragma once
#include <acpi/acpi.hpp>
#include <libc/stdint.hpp>

// ================================================ //
//         Interrupt Controller Structures          //
//                ACP, Table 5-45                   //
// ================================================ //

namespace madt::ics {

enum Type {
	CPU_LAPIC			= 0x00,
	IO_APIC				= 0x01,
	LAPIC_ADDR_OVERRIDE = 0x05
};

// ACP, under table 5-44
struct __attribute__((packed)) Header {
	u8 type;
	u8 length;
};

// ACP, Table 5-47
struct __attribute__((packed)) CPU_LAPIC_Flags {
	u8 enabled		  : 1;
	u8 online_capable : 1;
	u32 reserved	  : 30;
};

// ACP, Table 5-46
struct __attribute__((packed)) CPU_LAPIC_Struct {
	Header header;
	u8 acpi_cpu_uid;
	u8 apic_id;
	CPU_LAPIC_Flags flags;
};

// ACP, Table 5-48
struct __attribute__((packed)) IO_APIC_Struct {
	Header header;
	u8 io_apic_id;
	u8 reserved;
	u32 io_apic_addr;
	u32 gsib;
};

Header *get_header(Type type);

}; // namespace madt::ics

namespace madt {

// ACP, Table 5-44
struct __attribute__((packed)) Flags {
	u8 pcat_compat : 1;
	u32 reserved   : 31;
};

// ACP, Table 5-43
struct __attribute__((packed)) SDT_Table {
	acpi::sdt::Header header;
	u32 lapic_addr;
	Flags flags;
	ics::Header ics_table[];
};

void dbg_print_lapic_info();

}; // namespace madt