#include <acpi/apic/lapic.hpp>
#include <cpu/cpu.hpp>
#include <cpu/mmio.h>
#include <kernel.hpp>
#include <kutils/bits.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

namespace lapic {

void write_msr(u64 value) {
	// Write into APIC MSR
	// IM3, Figure 10-5. IA32_APIC_BASE MSR
	cpu::write_msr(BASE_MSR, value);
}

u64 read_msr() {
	// Get APIC MSR value
	// IM3, Figure 10-5. IA32_APIC_BASE MSR
	return cpu::read_msr(BASE_MSR);
}

u32 read_reg(u16 reg) {
	return mmio::read32((void *) kernel.acpi_tables.madt->lapic_addr + reg);
}

void write_reg(u16 reg, u32 val) {
	mmio::write32((void *) kernel.acpi_tables.madt->lapic_addr + reg, val);
}

u8 get_init_id() {
	// Get init APIC id from CPUID
	// IM3, 10.4.6 Local APIC ID
	u32 ebx = 0;
	cpu::get_cpuid(0x1, NULL, &ebx, NULL, NULL);
	ebx >>= 24;

	return (u8) ebx;
}

void enable() {
	// IM3, 10.4.3 Enabling or Disabling the Local APIC
	// Enable Local APIC by setting `enable` bit in MSR register
	u64 msr = cpu::read_msr(BASE_MSR);
	msr		= set_bit(msr, 11);
	write_msr(msr);

	// Set APIC software `enable` bit in spurious vector register
	u32 reg = read_reg(Reg::SPURIOUS);
	reg		= set_bit(reg, 8);
	write_reg(Reg::SPURIOUS, reg);
}

void set_spurious_interrupt() {
	// Set spurious interrupt handler and `enabled` byte
	// IM3, 10.9 Spurious interrupt
	lapic::write_reg(Reg::SPURIOUS, Interrupt::SPURIOUS);
}

u8 get_id() {
	// IM3, 10.4.6 Local APIC ID
	return read_reg(Reg::ID) >> 24;
}

u8 get_version() {
	// IM3, 10.4.8 Local APIC Version Register
	return (u8) read_reg(Reg::VERSION);
}

void write_eoi() {
	write_reg(Reg::EOI, 0xff);
}

void init() {
	// Clear priority register
	lapic::write_reg(TPR, 0x0);

	set_spurious_interrupt();
	enable();
}

}; // namespace lapic