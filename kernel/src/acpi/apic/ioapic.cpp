#include <acpi/apic/ioapic.hpp>
#include <cpu/mmio.h>
#include <kernel.hpp>
#include <libc/stdint.hpp>

namespace ioapic {

void set_ioregsel(u8 reg_id) {
	// IOA, 3.1.1. I/O Register Select Register
	mmio::write32((void *) (u64) kernel.acpi_tables.ioapic->io_apic_addr, (u32) reg_id);
}

void set_iowin(u32 val) {
	// IOA, 3.1.2. I/O Window Register
	mmio::write32((void *) ((u64) kernel.acpi_tables.ioapic->io_apic_addr + IOWIN), val);
}

u32 get_iowin() {
	return mmio::read32((void *) ((u64) kernel.acpi_tables.ioapic->io_apic_addr + IOWIN));
}

u32 IOAPIC_get_id(void) {
	// IOA, 3.2.1. IOAPIC Identification Register
	set_ioregsel(IOAPICID);
	auto iowin = get_iowin();
	iowin <<= 4;
	iowin >>= 28;

	return (u8) iowin;
}

u32 __get_version_reg(void) {
	// IOA, 3.2.2. IOAPIC Version Register
	set_ioregsel(IOAPICVER);

	return get_iowin();
}

u8 get_version(void) {
	// Get implementation version IOAPIC
	u32 iowin = __get_version_reg();

	return (u8) iowin;
}

u8 get_entries_num(void) {
	// Get Maximum Redirection Entry number.
	// This is number of interrupt input pins for the IOAPIC.
	// Range of values: 0-239
	u32 iowin = __get_version_reg();

	return (u8) (iowin >> 16) + 1;
}

void set_redir_entry(u8 entry_id, RedirEntry &entry) {
	// Set redirection entry (counting from 0).
	// IOA, 3.2.2. I/O Redirection Table Registers
	u8 addr_off = IOREDTBL + (entry_id * 2);
	set_ioregsel(addr_off);
	set_iowin(entry.lower_half);

	set_ioregsel(addr_off + 1);
	set_iowin(entry.upper_half);
}

RedirEntry get_redir_entry(u8 entry_id) {
	// Get redirection entry (counting from 0).
	u8 addr_off = IOREDTBL + (entry_id * 2);
	set_ioregsel(addr_off);
	RedirEntry entry = {.lower_half = get_iowin()};
	set_ioregsel(addr_off + 1);
	entry.upper_half = get_iowin();

	return entry;
}

void init() {
	// IRQ1 -> ISR36
	RedirEntry entry;
	entry.intvec	  = Interrupt::TIMER;
	entry.delmod	  = FIXED;
	entry.destmod	  = PHYSICAL;
	entry.intpol	  = Polarity::HIGH;
	entry.int_mask	  = 0;
	entry.trigmod	  = TriggerMode::EDGE;
	entry.destination = 0;

	set_redir_entry(IRQ::HPET, entry);
}

} // namespace ioapic