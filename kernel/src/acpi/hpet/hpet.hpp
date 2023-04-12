/*
	HPET (High Precision Event Timer)

	Specification URL:
	https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/software-developers-hpet-spec-1-0a.pdf

	HPET MMIO registers: HPT, Table 2. Memory-Mapped Registers
*/
#pragma once
#include <acpi/acpi.hpp>
#include <libc/stdint.hpp>

namespace hpet {

extern size g_clk_period_ns;

enum Reg {
	// General HPET registers offset
	// See: Table 2 Memory-Mapped Registers
	CAP_REG		   = 0x000,
	CONFIG_REG	   = 0x010,
	INT_STATUS_REG = 0x020,
	COUNTER_REG	   = 0x0f0
};

// See: Figure 3 General Capability and ID Register
struct __attribute__((packed)) CapReg {
	u8 rev_id;
	u8 num_tim_cap : 5;
	u8 count_size_cap : 1;
	u8 reserved1 : 1;
	u8 leg_route_cap : 1;
	u16 vendor_id;
	u32 counter_clk_period;
};

// See: Figure 4 General Configuration Register
struct __attribute__((packed)) ConfigReg {
	u8 enable_cnf : 1; // Global enable timer flag
	u8 leg_rt_cnf : 1;
	u64 reserved1 : 62;
};

// See: Table 3. HPET Description Table
struct __attribute__((packed)) SDT_Table {
	acpi::sdt::Header header;
	u32 event_timer_block_id;
	acpi::GAS base_address;
	u8 hpet_number;
	u16 min_clock_tick_periodic_mode;
	u8 page_protection;
};

void init();
SDT_Table *get_sdt_table();
void enable_counter();
void disable_counter();
bool is_counter_enabled();
u64 get_counter();
u8 get_no_timers();
u64 get_mmio_sz();

}; // namespace hpet