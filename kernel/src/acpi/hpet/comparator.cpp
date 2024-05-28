#include <acpi/apic/ioapic.hpp>
#include <acpi/hpet/comparator.hpp>
#include <acpi/hpet/hpet.hpp>
#include <cpu/mmio.h>
#include <kernel.hpp>
#include <kutils/assertions.hpp>
#include <kutils/console.hpp>

namespace hpet::comparator {

Comparator::Comparator(u8 timer_id) : __id(timer_id){
	ASSERT(g_clk_period_ns != 0);
};

void *Comparator::get_compar_reg_ptr(u64 reg) {
	// Return pointer to the HPET n-comparator block
	auto timers	  = (volatile MMIO *) (kernel.acpi_tables.hpet->base_address.address + FIRST_OFF);
	u64 base_addr = (u64) (timers + this->__id);

	return (void *) (base_addr + reg);
}

void Comparator::write_reg(u64 reg, u64 val) {
	void *ptr = this->get_compar_reg_ptr(reg);
	mmio::write64(ptr, val);
}

u64 Comparator::read_reg(u64 reg) {
	void *ptr = this->get_compar_reg_ptr(reg);

	return mmio::read64(ptr);
}

void Comparator::set_config(ConfigReg &reg) {
	// Set periodic or non-periodic type of HPET timer
	this->write_reg(CONFIG, *reinterpret_cast<u64 *>(&reg));
}

void Comparator::set_compar_value(u64 val) {
	// Read: 2.3.9.1 Register Definition and Usage Model
	this->write_reg(COMPAR, val);
}

u64 Comparator::get_compar_value() {
	// Get comparator of HPET timer
	return this->read_reg(COMPAR);
}

u32 Comparator::get_available_ioapic_routes() {
	// HPT, Tn_INT_ROUTE_CAP field
	return this->read_reg(Reg::CONFIG) >> 32;
}

static inline size __calc_ms_to_compar(u64 counter, size ms_time) {
	// Calculate comparator value basing

	// Convert miliseconds to nanoseconds per tick
	return counter + (ms_time * 1000000) / g_clk_period_ns;
}

void Comparator::set_one_shot(size ms_time) {
	// Set timer to one-shot mode. It fires interrupt only once.
	// IMPORTANT: This function assumes that main counter is already enabled.
	//
	// :ms_time  - amount of miliseconds till interrupt
	ConfigReg config_reg = {
		.int_type_cnf  = 0,
		.int_enb_cnf   = 1,
		.type_cnf	   = ONE_SHOT,
		.val_set_cnf   = 0,
		.mode32_cnf	   = 0,
		.int_route_cnf = ioapic::IRQ::HPET,
		.fsb_en_cnf	   = 0,
	};
	this->set_compar_value(__calc_ms_to_compar(hpet::get_counter(), ms_time));
	this->set_config(config_reg);
}

void Comparator::set_periodic(size ms_time) {
	// Set timer to periodic mode. It fires interrupt every period of time.
	// IMPORTANT: This function resets the main counter!
	//
	// :ms_time  - amount of miliseconds till interrupt
	hpet::disable_counter();
	ConfigReg config_reg = {
		.int_type_cnf  = 0,
		.int_enb_cnf   = 1,
		.type_cnf	   = PERIODIC,
		.val_set_cnf   = 1,
		.mode32_cnf	   = 0,
		.int_route_cnf = ioapic::IRQ::HPET,
		.fsb_en_cnf	   = 0,
	};
	this->set_config(config_reg);
	this->set_compar_value(__calc_ms_to_compar(0, ms_time));
	enable_counter();
}

}; // namespace hpet::comparator
