#include <acpi/acpi.hpp>
#include <acpi/hpet/hpet.hpp>
#include <cpu/mmio.h>
#include <kernel.hpp>
#include <libc/stdlib.hpp>

namespace hpet {

size g_clk_period_ns = 0; // Number of nanoseconds per 1 tick

void *__get_reg_ptr(u64 reg) {
	// Get pointer to the specific HPET register
	return (void *) ((u64) kernel.acpi_tables.hpet->base_address.address + reg);
}

u64 read_reg(u64 reg) {
	void *ptr = __get_reg_ptr(reg);
	return mmio::read64(ptr);
}

void write_reg(u64 reg, u64 val) {
	void *ptr = __get_reg_ptr(reg);
	mmio::write64(ptr, val);
}

u32 get_counter_clk_period() {
	// The returned value is in femtoseconds
	// Number of femtoseconds per 1 tick
	return read_reg(Reg::CAP_REG) >> 32;
}

void init() {
	// Convert femtoseconds to nanoseconds
	g_clk_period_ns = get_counter_clk_period() / 1000000;
}

void enable_counter() {
	// Set to 0 and start
	write_reg(COUNTER_REG, 0);
	u64 reg			   = read_reg(CONFIG_REG);
	auto config		   = (ConfigReg *) &reg;
	config->enable_cnf = 1;
	write_reg(CONFIG_REG, reg);
}

void disable_counter() {
	// Stop and reset to 0
	u64 reg			   = read_reg(CONFIG_REG);
	auto config		   = (ConfigReg *) &reg;
	config->enable_cnf = 0;
	write_reg(CONFIG_REG, reg);
	write_reg(COUNTER_REG, 0);
}

SDT_Table *get_sdt_table() {
	return (SDT_Table *) acpi::sdt::get_header(acpi::sdt::HPET_SIG);
}

bool is_counter_enabled() {
	// Check if counter is running
	u64 reg		= read_reg(CONFIG_REG);
	auto config = (ConfigReg *) &reg;

	return config->enable_cnf;
}

u64 get_counter() {
	// Get value from main counter
	return read_reg(COUNTER_REG);
}

u8 get_no_comparators() {
	// Get number of available HPET comparators
	u64 reg	 = read_reg(CAP_REG);
	auto cap = (CapReg *) &reg;

	return cap->num_tim_cap + 1;
}

u64 get_mmio_sz() {
	/*
		Get size of HPET entire MMIO space in bytes
		Calculate it by adding up width of all HPET registers
		See: Table 2 Memory-Mapped Registers
	*/

	// General registers = 8 registers * 8 bytes
	u32 result = 8 * 8;

	// result + number_of_comparators * bytes_per_timer
	return result + get_no_comparators() * (4 * 8);
}

}; // namespace hpet